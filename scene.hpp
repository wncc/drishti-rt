#ifndef __SCENE_HPP_INCLUDED__
#define __SCENE_HPP_INCLUDED__

#include "camera.hpp"
#include "image.hpp"
#include "materials.hpp"
#include "objects.hpp"
#include "lights.hpp"
#include "ray.hpp"
#include "integrator.hpp"
#include <vector>
#include <memory>

//using namespace std;

class scene{
	camera cam;
    image img;
    integrator intg;
    std::vector<std::shared_ptr<material>> materialslist;
    std::vector<std::shared_ptr<object> > objectslist;
    std::vector<light> lightslist;
    std::vector<std::vector<double> > rotation_mat;
    std::vector<std::vector<double> > translation_mat;
    std::vector<std::vector<double> > inv_rotation_mat;
    std::vector<std::vector<double> > inv_translation_mat;

public:
	void setCamera(camera cam0);
	void setImage(image img0);
	void setIntegrator(integrator intg0);
	void setMaterials(std::vector<std::shared_ptr<material>> materials);
	void setObjects(std::vector<std::shared_ptr<object> > objects);
	void setLights(std::vector<light> lights);
	camera getCamera();
	image getImage();
	integrator getIntegrator();
	std::vector<std::shared_ptr<material>> getMaterials();
	std::vector<std::shared_ptr<object> > getObjects();
	std::vector<light> getLights();
	void rotation_matrix_formation();
	void translation_matrix_formation();
	void inv_translation_matrix_formation();
	void inv_rotation_matrix_formation();
	std::vector<double> world_to_camera(std::vector<double> world_c);
	std::vector<double> camera_to_world(std::vector<double> camera_c);
	std::shared_ptr<object> intersect(ray Ray);
};

#endif