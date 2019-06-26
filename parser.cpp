#include <exception>
#include <stdexcept> // std::runtime_error
#include <cstdlib> // EXIT_FAILURE
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "inc/tinyxml/tinyxml.h"
#include "scene.hpp"
#include "camera.hpp"
#include "image.hpp"
#include "materials.hpp"
#include "objects.hpp"
#include "lights.hpp"

//using namespace std;


void write_to_ppm(int ***arr,double w,double h)
{
    std::ofstream img ("picture.ppm");
    img << "P6" <<std::endl;
    img << w <<" "<< h <<std::endl;
    img << "255" <<std::endl;

    for(int i=0;i<h;i++)
    {
        for(int j=0;j<w;j++)
        {
            double r = arr[j][i][0];
            double g = arr[j][i][1];
            double b = arr[j][i][2];

            img << r <<" " << g <<" "<< b << std::endl;
        }
    }

}


const char* textValue(TiXmlElement* e)
{
    TiXmlNode* first=e->FirstChild();
    if(first!=0 && first==e->LastChild() && first->Type()==TiXmlNode::TINYXML_TEXT)
        return first->Value();
    else
        throw std::runtime_error(std::string("bad ") + e->Value( ) + " element");
}

std::vector<double> nodeToVector(TiXmlElement* Vect)
{
    std::string vect = textValue(Vect);
    std::stringstream ss(vect);
    double xval; ss>>xval;
    double yval; ss>>yval;
    double zval; ss>>zval;

    std::vector<double> result_vect;
    result_vect.push_back(xval);
    result_vect.push_back(yval);
    result_vect.push_back(zval);
}

double doubleVal(TiXmlElement* dval, const char* attrib)
{
    std::string temp_str = dval->Attribute(attrib);
    char* str = new char[temp_str.size()];
    strcpy(str, temp_str.c_str());

    std::stringstream ss(str);
    double result; ss>>result;
    
    delete str;
    return result;
}

camera nodeToCamera(TiXmlElement* Camera)
{
    camera result;
    TiXmlElement* element = Camera->FirstChildElement();
    if(element && strcmp(element->Value(),"lookat")==0)
        result.setLookat(nodeToVector(element));

    element=element->NextSiblingElement();
    if(element && strcmp(element->Value(),"eye")==0)
        result.setEye(nodeToVector(element));

    element=element->NextSiblingElement();
    if(element && strcmp(element->Value(),"up")==0)
        result.setUp(nodeToVector(element));
    
    result.setThird();

    element=element->NextSiblingElement();
    if(element && strcmp(element->Value(),"fov")==0)
        result.setFov(doubleVal(element,"angle"));

    element=element->NextSiblingElement();
    if(element && strcmp(element->Value(),"near")==0)
        result.setNear(doubleVal(element,"double"));
    
    element=element->NextSiblingElement();
    if(element && strcmp(element->Value(),"far")==0)
        result.setFar(doubleVal(element,"double"));

    return result;
}

image nodeToImage(TiXmlElement* Image)
{
	image result;
	TiXmlElement* element = Image->FirstChildElement();
	if(element && strcmp(element->Value(),"width")==0)
		result.setWidth(doubleVal(element,"width"));
	
    element=element->NextSiblingElement();
	if(element && strcmp(element->Value(),"height")==0)
		result.setHeight(doubleVal(element,"height"));
	
    element=element->NextSiblingElement();
	if(element && strcmp(element->Value(),"bgcolor")==0)
		result.setBgcolor(nodeToVector(element));
	
	return result;
}

material* nodeToMaterial(TiXmlElement* Material)
{
	material *result;

    const char *id;
    if((id = Material->Attribute("id")) && strcmp(Material->Value(),"simplemat")==0) //simplemat material
    {
        simplemat sim_mat(id); //creating a simplemat object
        
        TiXmlElement* element = Material->FirstChildElement();

        if(element && strcmp(element->Value(),"diffuse")==0)
            sim_mat.setDiffuse(nodeToVector(element));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"specular")==0)
            sim_mat.setSpecular(nodeToVector(element));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"reflect")==0)
            sim_mat.setReflect(nodeToVector(element));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"transmit")==0)
            sim_mat.setTransmit(nodeToVector(element));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"eta")==0)
            sim_mat.setEta(doubleVal(element,"eta"));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"n")==0)
            sim_mat.setN(doubleVal(element,"n"));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"isreflect")==0)
            sim_mat.setIsreflect(strcmp(element->Attribute("isreflect"),"true"));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"istransmit")==0)
            sim_mat.setIstransmit(strcmp(element->Attribute("istransmit"),"true"));

        result = &sim_mat;
    }
    else
        throw std::runtime_error(std::string("bad ") + " element");

	return result;
}

std::shared_ptr<object> nodeToObject(TiXmlElement* Object, scene scene_obj) //INCOMPLETE!
{
    const char* mat_name;
    if((mat_name = Object->Attribute("material")) && strcmp(Object->Value(),"sphere")==0) //sphere object
    {
        std::vector<material> materialslist = scene_obj.getMaterials();
        std::vector<std::string> mat_names;//to store the ID of the materials in materialslist
        for (std::vector<material>::iterator it = materialslist.begin() ; it != materialslist.end(); ++it)
        {
            mat_names.push_back((*it).getID());//extracting material IDs
        }
        std::string search_mat(mat_name);//material ID to be searched (from scene file)
        std::vector<std::string>::iterator it_str;
        it_str =  find(mat_names.begin(),mat_names.end(),search_mat);
        
        sphere* sph_obj = new sphere();

        if(it_str!=mat_names.end())
        {
            sph_obj->setMaterial(*it_str);//calls the superclass function
        }
        else
            throw std::runtime_error(std::string("material ") + " for sphere object not found");

        TiXmlElement* element = Object->FirstChildElement();

        if(element && strcmp(element->Value(),"center")==0)
            sph_obj->setCenter(nodeToVector(element));
        
        element=element->NextSiblingElement();
        if(element && strcmp(element->Value(),"radius")==0)
            sph_obj->setRadius(doubleVal(element,"radius"));
        
        return std::shared_ptr<object> (sph_obj);
    }
    else
        throw std::runtime_error(std::string("bad ") + " element");

}

light* nodeToLight(TiXmlElement* Light)
{
	light *result;

	if(strcmp(Light->Value(),"pointlight")==0) //point light source
	{
		pointlight plight;
		TiXmlElement* element = Light->FirstChildElement();

        if(element && strcmp(element->Value(),"position")==0)
            plight.setPos(nodeToVector(element));
        
        element=element->NextSiblingElement();

        if(element && strcmp(element->Value(),"color")==0)
            plight.setColor(nodeToVector(element));
        
        element=element->NextSiblingElement();

        if(element && strcmp(element->Value(),"ka")==0)
            plight.setKa(doubleVal(element,"ka"));
        
        result = &plight;

	}
	else
		throw std::runtime_error(std::string("bad ") + " element");

	return result;
}

integrator* nodeToIntegrator(TiXmlElement* Integrator)
{
    integrator *result;
    if(strcmp(Integrator->Value(),"whitted")==0)//whitted integrator
    {
        whitted whit_intg;
        TiXmlElement* element = Integrator->FirstChildElement();

        if(element && strcmp(element->Value(),"depth-of-recursion")==0)
            whit_intg.setDepth(int(doubleVal(element,"depth-of-recursion")));
        
        result = &whit_intg;
    }
    else
        throw std::runtime_error(std::string("bad ") + " element");


    return result;
}

int main(){
    TiXmlDocument doc("./scenes/sample-scene.xml");
    
    scene scene_obj;
    vector<material> materialslist;
    vector<shared_ptr<object>> objectslist;
    vector<light> lightslist;

    if(!doc.LoadFile())
        throw std::runtime_error("bad parse");

    TiXmlElement* root=doc.RootElement();
    if(strcmp(root->Value(),"rt-scene")!=0)
        throw std::runtime_error(std::string("bad root: ")+ root->Value());
        
    for(TiXmlElement* a=root->FirstChildElement();a;a=a->NextSiblingElement())
    {
        if(strcmp(a->Value(),"camera")==0)
            scene_obj.setCamera(nodeToCamera(a));
        else if(strcmp(a->Value(),"image")==0)
            scene_obj.setImage(nodeToImage(a));
        else if(strcmp(a->Value(),"materials")==0)
        {
        	for(TiXmlElement* b=a->FirstChildElement();b;b->NextSiblingElement())
        	{
        		materialslist.push_back(*(nodeToMaterial(b)));
        	}
            scene_obj.setMaterials(materialslist);
        }
        else if(strcmp(a->Value(),"objects")==0)
        {    
        	for(TiXmlElement* b=a->FirstChildElement();b;b->NextSiblingElement())
        	{
        		objectslist.push_back(nodeToObject(b,scene_obj));
        	}
            scene_obj.setObjects(objectslist);
        }	
        else if(strcmp(a->Value(),"lights")==0)
        {
           for(TiXmlElement* b=a->FirstChildElement();b;b->NextSiblingElement())
        	{
        		lightslist.push_back(*(nodeToLight(b)));
        	}
            scene_obj.setLights(lightslist);
        }	
        else if(strcmp(a->Value(),"integrator")==0)
            scene_obj.setIntegrator(*(nodeToIntegrator(a)));

    }

    scene_obj.rotation_matrix_formation();
    scene_obj.translation_matrix_formation();
    scene_obj.inv_translation_matrix_formation();
    scene_obj.inv_rotation_matrix_formation();

    int w = scene_obj.getImage().getWidth();
    int h = scene_obj.getImage().getHeight();

    int ***img_arr;
    img_arr = new int **[w];
    for(int i=0;i<w;i++)
    {
        img_arr [i] = new int*[h];
        for(int j=0;j<h;j++)
            img_arr[i][j] = new int[3];
    }

    std::vector<double> v = scene_obj.getImage().getBgcolor();
    for(int i=0;i<w;i++)
    {
        for(int j=0;j<h;j++)
        {
            img_arr[i][j][0] = v[0];
            img_arr[i][j][1] = v[1];
            img_arr[i][j][2] = v[2];
        }
    }

    write_to_ppm(img_arr,w,h);
}