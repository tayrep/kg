#include "Image.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <stdexcept>
namespace filters {
static int gray(const Image& a,int x,int y){int r,g,b;a.getColor(x,y,r,g,b);if(a.nChannels()<=2)return r;return static_cast<int>(std::lround(.299*r+.587*g+.114*b));}
static void put(Image& a,int x,int y,double v){int k=std::clamp(static_cast<int>(std::lround(v)),0,255);a.setColor(x,y,k,k,k);}
Image toGray(const Image& a){Image out(a.width(),a.height(),1);for(int y=0;y<a.height();++y)for(int x=0;x<a.width();++x)put(out,x,y,gray(a,x,y));return out;}
// All filter windows must fit entirely inside the source image. Border remains black.
Image sobel(const Image& a){Image out(a.width(),a.height(),1);const int gx[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}},gy[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};for(int y=1;y<a.height()-1;++y)for(int x=1;x<a.width()-1;++x){int sx=0,sy=0;for(int j=-1;j<=1;++j)for(int i=-1;i<=1;++i){int v=gray(a,x+i,y+j);sx+=gx[j+1][i+1]*v;sy+=gy[j+1][i+1]*v;}put(out,x,y,std::hypot(sx,sy));}return out;}
Image roberts(const Image& a){Image out(a.width(),a.height(),1);for(int y=0;y<a.height()-1;++y)for(int x=0;x<a.width()-1;++x){int p=gray(a,x,y),q=gray(a,x+1,y+1),r=gray(a,x+1,y),s=gray(a,x,y+1);put(out,x,y,std::hypot(p-q,r-s));}return out;}
Image laplace(const Image& a){Image out(a.width(),a.height(),1);for(int y=1;y<a.height()-1;++y)for(int x=1;x<a.width()-1;++x){int v=4*gray(a,x,y)-gray(a,x-1,y)-gray(a,x+1,y)-gray(a,x,y-1)-gray(a,x,y+1);put(out,x,y,std::abs(v));}return out;}
static void validate(int k){if(k<1||k%2==0)throw std::invalid_argument("Kernel size must be positive and odd");}
Image meanFilter(const Image& a,int k=3){validate(k);Image out(a.width(),a.height(),1);int r=k/2;for(int y=r;y<a.height()-r;++y)for(int x=r;x<a.width()-r;++x){double sum=0;for(int j=-r;j<=r;++j)for(int i=-r;i<=r;++i)sum+=gray(a,x+i,y+j);put(out,x,y,sum/(k*k));}return out;}
Image gaussFilter(const Image& a,int k=3,double sigma=1.0){validate(k);if(sigma<=0)throw std::invalid_argument("Sigma must be positive");Image out(a.width(),a.height(),1);int r=k/2;std::vector<double> weights(k*k);double norm=0;for(int j=-r;j<=r;++j)for(int i=-r;i<=r;++i){double w=std::exp(-(i*i+j*j)/(2*sigma*sigma));weights[(j+r)*k+i+r]=w;norm+=w;}for(int y=r;y<a.height()-r;++y)for(int x=r;x<a.width()-r;++x){double sum=0;for(int j=-r;j<=r;++j)for(int i=-r;i<=r;++i)sum+=gray(a,x+i,y+j)*weights[(j+r)*k+i+r];put(out,x,y,sum/norm);}return out;}
static Image order(const Image& a,int k,int type){validate(k);Image out(a.width(),a.height(),1);int r=k/2;std::vector<int> v;v.reserve(k*k);for(int y=r;y<a.height()-r;++y)for(int x=r;x<a.width()-r;++x){v.clear();for(int j=-r;j<=r;++j)for(int i=-r;i<=r;++i)v.push_back(gray(a,x+i,y+j));if(type==0){std::nth_element(v.begin(),v.begin()+v.size()/2,v.end());put(out,x,y,v[v.size()/2]);}else if(type==1)put(out,x,y,*std::min_element(v.begin(),v.end()));else put(out,x,y,*std::max_element(v.begin(),v.end()));}return out;}
Image medianFilter(const Image& a,int k=3){return order(a,k,0);}
Image minFilter(const Image& a,int k=3){return order(a,k,1);}
Image maxFilter(const Image& a,int k=3){return order(a,k,2);}
}
