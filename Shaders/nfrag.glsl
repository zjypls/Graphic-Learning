#version 450
out vec4 FragColor;
in vec2 fpos;
#define LAMBERT 0
#define MIRR 1
//#define METAL 2
#define LIGHT 2
#define GLASS 3

struct Ray{
    vec3 origin;
    vec3 direction;
    vec3 color;
};
struct Sphere{
    vec3 center;
    float radius;
    int type;
    vec3 color;
};
struct Face{
    float xmin,xmax,ymin,ymax,zmin,zmax;
    int type;
    vec3 Normal;
    vec3 Point;
    vec3 color;
};
struct HitRec{
    float t;
    int type;
    vec3 Normal;
    vec3 position;
    vec3 color;
};
struct Camera{
    vec3 origin;
    vec3 horizon;
    vec3 vertical;
    vec3 leftcorner;
};
struct World{
    int count1,count2;
    Sphere[5] spheres;
    Face[5] faces;
};
uniform World world;
uniform Camera camera;
uniform int depth;
uniform float msaa;
uniform int Rdu;
uniform sampler2D bef;
//uniform vec3 FrameSize;
vec3 ligpos=vec3(0,2,-1);
vec3 point_in_ray(Ray ray,float t){
    return ray.origin+t*ray.direction;
}

void FaceHit(Ray ray,Face face,inout HitRec rec){
    float dir_N=dot(ray.direction,face.Normal);
    if(abs(dir_N)<0.001)
        return;
    float root=-dot(ray.origin-face.Point,face.Normal)/dir_N;
    if(root<0.001||root>rec.t)
        return;
    vec3 point=point_in_ray(ray,root);
    if(point.x<face.xmin||point.x>face.xmax||point.y<face.ymin||point.y>face.ymax||point.z<face.zmin||point.z>face.zmax)
        return;
    rec.t=root;
    rec.Normal=face.Normal;
    rec.type=face.type;
    rec.position=point;
    rec.color=face.color;
}

void SphereHit(Ray ray,Sphere sphere,inout HitRec rec ){
    vec3 oc=ray.origin-sphere.center;
    float a=dot(ray.direction, ray.direction);
    float b=-dot(ray.direction, oc);
    float c=dot(oc, oc)-sphere.radius*sphere.radius;
    float dist=b*b-(a*c);
    if (dist<0)
        return ;
    dist=sqrt(dist);
    float root=(b-dist)/a;
    if (root>0.001&&root<rec.t){
        rec.t=root;
        rec.position=point_in_ray(ray,root);
        rec.Normal=(rec.position-sphere.center)/sphere.radius;
        rec.type=sphere.type;
        rec.color=sphere.color;
    }
//    if(sphere.type!=GLASS)
//        return;
//    root=(b+dist)/a;
//    if(root>0.001&&root<rec.t){
//        rec.t=root;
//        rec.position=point_in_ray(ray,root);
//        rec.Normal=(rec.position-sphere.center)/sphere.radius;
//        rec.type=sphere.type;
//        rec.color=sphere.color;
//    }
}

bool IsHit(Ray ray,inout HitRec rec){
    for(int i=0;i<world.count1;++i){
        SphereHit(ray,world.spheres[i],rec);
    }
    for(int i=0;i<world.count2;++i){
        FaceHit(ray,world.faces[i],rec);
    }
    return rec.t<10000;
}

uint seed=uint(fpos.y*(camera.vertical.y*200)*(camera.horizon.x*200)+fpos.x*(camera.horizon.x*200));
uint Cnt=seed*Rdu;
float rand(){
    seed=(seed*(1103515245)+1)%2147483648;
    return (float(seed)/2147483648.0);
}
float RandXY(float x, float y){
    return (fract(cos(x * (12.9898) + y * (4.1414)) * 43758.5453)-0.5)*2;
}
float Rand(){
    float a = RandXY(fpos.x, rand());
    float b = RandXY(rand(), fpos.y);
    float c = RandXY(Cnt++, rand());
    float d = RandXY(rand(), a);
    float e = RandXY(b, c);
    float f = RandXY(d, e);

    return f;
}

uint wseed=seed+Cnt;
float randcore(uint Seed){
    Seed =(Seed^uint(61))^(Seed >> uint (16));
    Seed*= uint (9);
    Seed = Seed ^ ( Seed >> uint (4) ) ;
    Seed*= uint (0x27d4eb2d);
wseed = Seed ^ ( Seed >> uint (15) ) ;
return float(wseed)*(1.0/4294967296.0);
}
float RandN () {return (randcore (wseed)-0.5)*2;}


vec3 GetUnitVec(in vec3 Normal){
    vec3 res=vec3(RandN(),RandN(),RandN());
    if(dot(res,Normal)<0)
        res=-res;
    return res;
}
vec3 RefractMy(vec3 dir,vec3 Normal,float eta){
    float sinth=dot(-normalize(dir),Normal);
    if(sinth<0){
        sinth=-sinth;
        Normal=-Normal;
        eta=1/eta;
    }
    float sinth1=sinth*eta;
    if(sinth1>1)
        return reflect(dir,Normal);
    float costh=sqrt(1-sinth1*sinth1);
    float tanth1=sinth1/costh;
    vec3 res_h=tanth1*costh*cross(Normal,cross(dir,Normal));
    return costh*Normal+res_h;
}

vec3 Newrefract(vec3 uv,vec3 n,float etai_over_etat) {
float cos_theta =dot(-uv, n);
vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
vec3 r_out_parallel = -sqrt(abs(1.0 -length(r_out_perp)*length(r_out_perp))) * n;
return r_out_perp + r_out_parallel;
}

vec3 getColor(Ray ray){
    int dep=depth;
    float rate=1;
    vec3 color=vec3(1,1,1);
    HitRec rec=HitRec(10000,0,vec3(0),vec3(0),vec3(0));
    while(dep-->0&&IsHit(ray,rec)){
        switch(rec.type){
            case LAMBERT:
            color*=0.6*rec.color;
            ray=Ray(rec.position,normalize(GetUnitVec(rec.Normal)+normalize(ligpos-rec.position)),rec.color);
            break;
            case MIRR:
            color*=0.8*rec.color;
            ray=Ray(rec.position,reflect(ray.direction,rec.Normal),rec.color);
            break;
            case LIGHT:
            dep=0;
            color*=5*rec.color;
            rate=0;
            break;
            case GLASS:
            color*=0.9*rec.color;
            ray=Ray(rec.position,refract(ray.direction,rec.Normal,1/1.5f),rec.color);
        }
        rec.t=10000;
    }
    float y=((normalize(ray.direction)+1)/2).y;
    return ((1-y)*vec3(1,1,1)+y*vec3(0.5,0.7,1))*color;
}



void main() {
    float u=fpos.x,v=fpos.y;
    vec3 finalcolor=vec3(0);
    Ray ray;
    ray.origin=camera.origin;
    ray.color=vec3(1);
    for(int i=0;i<msaa;++i){
        ray.direction=normalize(camera.leftcorner+camera.horizon*(u+RandN()/1000)+camera.vertical*(v+RandN()/1000));
        finalcolor+=getColor(ray);
    }
    finalcolor/=msaa;
    FragColor=vec4(finalcolor,1)*(1.0f/Rdu)+texture(bef,fpos)*(float(Rdu-1)/Rdu);
}
