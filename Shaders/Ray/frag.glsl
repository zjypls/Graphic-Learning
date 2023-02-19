#version 450
out vec4 FragColor;
in vec2 fpos;
struct Ray{
    vec3 origin;
    vec3 direction;
    vec3 color;
};
struct Sphere{
    vec3 center;
    float radius;
    int type;
};
struct Face{
    float xmin,xmax,ymin,ymax,zmin,zmax;
    int type;
    vec3 Normal;
    vec3 Point;
};
struct HitRec{
    float t;
    int type;
    vec3 Normal;
    vec3 position;
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

vec3 point_in_ray(Ray ray,float t){
    return ray.origin+t*ray.direction;
}

HitRec FaceHit(Ray ray,Face face){
    HitRec rec=HitRec(10000,0,vec3(0),vec3(0));
    float dir_N=dot(ray.direction,face.Normal);
    if(abs(dir_N)<0.001)
        return rec;
    float root=dot(ray.origin-face.Point,face.Normal)/dir_N;
    if(root<0.001||root>rec.t)
        return rec;
    vec3 point=point_in_ray(ray,root);
    if(point.x<face.xmin||point.x>face.xmax||point.y<face.ymin||point.y>face.ymax||point.z<face.zmin||point.z>face.zmax)
        return rec;
    rec.t=root;
    rec.Normal=face.Normal;
    rec.type=face.type;
    rec.position=point;
    return rec;
}

HitRec SphereHit(Ray ray,Sphere sphere){
    HitRec rec=HitRec(10000,0,vec3(0),vec3(0));
    vec3 oc=ray.origin-sphere.center;
    float a=dot(ray.direction, ray.direction);
    float b=-dot(ray.direction, oc);
    float c=dot(oc, oc)-sphere.radius*sphere.radius;
    float dist=sqrt(b*b-(a*c));
    if (dist<0)
        return rec;
    float root=(b-dist)/a;
    if (root>0.001&&root<rec.t){
        rec.t=root;
        rec.position=point_in_ray(ray,root);
        rec.Normal=normalize(rec.position-sphere.center);
        rec.type=sphere.type;
        return rec;
    }
    root=(b+dist)/a;
    if(root>0.001&&root<rec.t){
        rec.t=root;
        rec.position=point_in_ray(ray,root);
        rec.Normal=normalize(rec.position-sphere.center);
        rec.type=sphere.type;
        //return true;
    }
    //return false;
    return rec;
}

HitRec IsHit(Ray ray){
    HitRec rec=HitRec(10000,0,vec3(0),vec3(0)),nrec=HitRec(10000,0,vec3(0),vec3(0));
    for(int i=0;i<world.count1;++i){
        nrec=SphereHit(ray,world.spheres[i]);
        if(nrec.t<rec.t)
            rec=nrec;
    }
    for(int i=0;i<world.count2;++i){
        nrec=FaceHit(ray,world.faces[i]);
        if(nrec.t<rec.t)
            rec=nrec;
    }
    return rec;
}

uint seed=uint(fpos.y*600*800+fpos.x*800);
uint Cnt=seed;
float rand(){
    seed=(seed*(1103515245)+1)%2147483648;
    return float(seed)/2147483648.0;
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

vec3 getColor(Ray ray){
    int dep=depth;
    vec3 color=vec3(1,1,1);
    HitRec rec=IsHit(ray);
    while(dep-->0&&rec.t<10000){
        //break;
        color*=0.7*ray.color;
        switch(rec.type){
            case 0:
                ray=Ray(rec.position, vec3(Rand(), Rand(), Rand())+rec.Normal, ray.color);
                break;
            case 1:
                ray=Ray(rec.position,reflect(ray.direction,rec.Normal),ray.color);
            break;
        }
        rec=IsHit(ray);
    }
    float y=((normalize(ray.direction)+1)/2).y;
    return ((1-y)*vec3(1,1,1)+y*vec3(0.5,0.7,1))*color;
    //return rec.Normal;
}




void main() {
    for(int i=0;i<10;++i)
        Rand();
    float u=fpos.x,v=fpos.y;
    vec3 finalcolor=vec3(0);
    Ray ray;
    for(int i=0;i<msaa;++i){
        ray=Ray(camera.origin,normalize(camera.leftcorner+camera.horizon*(u+Rand()/1000)+camera.vertical*(v+Rand()/1000)),vec3(1,1,1));
        finalcolor+=getColor(ray);
    }
    finalcolor/=msaa;
    FragColor=vec4(finalcolor,1);
}
