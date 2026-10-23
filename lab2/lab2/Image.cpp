#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <cstring>
#include <limits>
#include <algorithm>
Image::Image() = default;
Image::Image(int w,int h,int n) {
 if(w<=0||h<=0||n<1||n>4) throw std::invalid_argument("Invalid image dimensions/channels");
 size_t s=static_cast<size_t>(w)*h*n;
 if(s/static_cast<size_t>(n)/static_cast<size_t>(h)!=static_cast<size_t>(w)) throw std::overflow_error("Image size overflow");
 m_width=w;m_height=h;m_nChannels=n;m_size=s;m_data=std::make_unique<uint8_t[]>(s);std::memset(m_data.get(),0,s);
}
Image::Image(const std::string& path){if(!isRead(path))throw std::runtime_error("Cannot read: "+path);}
Image::Image(const Image& o):m_width(o.m_width),m_height(o.m_height),m_nChannels(o.m_nChannels),m_size(o.m_size){if(o.m_data){m_data=std::make_unique<uint8_t[]>(m_size);std::memcpy(m_data.get(),o.m_data.get(),m_size);}}
Image& Image::operator=(const Image& o){if(this!=&o){Image temp(o);std::swap(m_width,temp.m_width);std::swap(m_height,temp.m_height);std::swap(m_nChannels,temp.m_nChannels);std::swap(m_size,temp.m_size);m_data.swap(temp.m_data);}return *this;}
bool Image::isRead(const std::string& path){int w=0,h=0,n=0;unsigned char* p=stbi_load(path.c_str(),&w,&h,&n,0);if(!p)return false;if(w<=0||h<=0||n<1||n>4){stbi_image_free(p);return false;}size_t s=static_cast<size_t>(w)*h*n;auto data=std::make_unique<uint8_t[]>(s);std::memcpy(data.get(),p,s);stbi_image_free(p);m_width=w;m_height=h;m_nChannels=n;m_size=s;m_data=std::move(data);return true;}
bool Image::isWrite(const std::string& path)const{return m_data&&stbi_write_png(path.c_str(),m_width,m_height,m_nChannels,m_data.get(),m_width*m_nChannels)!=0;}
void Image::setColor(int x,int y,int r,int g,int b){if(!m_data||x<0||y<0||x>=m_width||y>=m_height)return;size_t i=(static_cast<size_t>(y)*m_width+x)*m_nChannels;auto clamp=[](int v){return static_cast<uint8_t>(std::clamp(v,0,255));};m_data[i]=clamp(r);if(m_nChannels>=3){m_data[i+1]=clamp(g);m_data[i+2]=clamp(b);}}
void Image::getColor(int x,int y,int& r,int& g,int& b)const{r=g=b=0;if(!m_data||x<0||y<0||x>=m_width||y>=m_height)return;size_t i=(static_cast<size_t>(y)*m_width+x)*m_nChannels;r=m_data[i];if(m_nChannels>=3){g=m_data[i+1];b=m_data[i+2];}}
