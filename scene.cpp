#include "scene.hpp"

//array*array
vector<vector<double> > mat_mult(vector<vector<double> > a,vector<vector<double> > b)
{
	int num_row1 = a.size();
	int num_column1 = a[0].size();

	int num_row2 = b.size();
	int num_column2 = b[0].size();


	vector<double> x(4,0);

	vector<vector<double> > c(4,x);

	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			for(int k=0;k<4;k++)
			{
				c[i][j]+=a[i][k]*b[k][j];
			}
		}
	}

	return c;


}

//array*vector
vector<double> mat_mult(vector<vector<double> > a,vector<double> b)
{
	
	vector<double> c(4,0);
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			c[i]+=a[i][j]*b[j];
		}

	}

	return c;
}

vector<vector<double> > transpose(vector<vector<double> > a)
{	vector<double> x(4,0);
	vector<vector<double> > v(4,x);

	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			v[j][i]=a[i][j];
		}
	}
		return v;
}

void scene::setCamera(camera cam0)
{
	cam = cam0;
}
void scene::setImage(image img0)
{
	img = img0;
}
void scene::setIntegrator(integrator intg0)
{
	intg = intg0;
}
void scene::setMaterials(vector<material> materials)
{
	materialslist = materials;
}
void scene::setObjects(vector<object> objects)
{
	objectslist = objects;
}
void scene::setLights(vector<light> lights)
{
	lightslist = lights;
}

camera scene::getCamera()
{
	return cam;
}
image scene::getImage()
{
	return img;
}
integrator scene::getIntegrator()
{
	return intg;
}
vector<material> scene::getMaterials()
{
	return materialslist;
}
vector<object> scene::getObjects()
{
	return objectslist;
}
vector<light> scene::getLights()
{
	return lightslist;
}

void scene::rotation_matrix_formation()
{
	vector<double> x = cam.getLookat();
	vector<double> z = cam.getUp();
	vector<double> y = cam.getThird();
	x.push_back(0);
	y.push_back(0);
	z.push_back(0);
	vector<double> fourth(3,0);  //supposed to be fourth row of matrix
	fourth.push_back(1);
	
	rotation_mat.push_back(x);
	rotation_mat.push_back(y);
	rotation_mat.push_back(z);
	rotation_mat.push_back(fourth);

}
void scene::inv_rotation_matrix_formation()
{
	vector<vector<double> > v = transpose(rotation_mat);
	inv_rotation_mat = v;
}

void scene::inv_translation_matrix_formation()
{
	vector<double> fourth = cam.getEye();
	vector<double> x;
	vector<double> y;
	vector<double> z;
	x.push_back(1);
	x.push_back(0);
	x.push_back(0);
	x.push_back(-1*fourth[0]);

	y.push_back(0);
	y.push_back(1);
	y.push_back(0);
	y.push_back(-1*fourth[1]);

	z.push_back(0);
	z.push_back(0);
	z.push_back(1);
	z.push_back(-1*fourth[2]);


	vector<double> a(3,0);
	a.push_back(1);

	inv_translation_mat.push_back(x);
	inv_translation_mat.push_back(y);
	inv_translation_mat.push_back(z);
	inv_translation_mat.push_back(a);


}
void scene::translation_matrix_formation()
{
	vector<double> fourth = cam.getEye();  //supposed to be fourth column of translation matrix
	vector<double> x;
	vector<double> y;
	vector<double> z;
	x.push_back(1);
	x.push_back(0);
	x.push_back(0);
	x.push_back(fourth[0]);

	y.push_back(0);
	y.push_back(1);
	y.push_back(0);
	y.push_back(fourth[1]);

	z.push_back(0);
	z.push_back(0);
	z.push_back(1);
	z.push_back(fourth[2]);

	vector<double> a(3,0);
	a.push_back(1);

	translation_mat.push_back(x);
	translation_mat.push_back(y);
	translation_mat.push_back(z);
	translation_mat.push_back(a);

}

vector<double> scene::world_to_camera(vector<double> world_c)
{
    vector<double> camera_c(world_c.size(),0);
    camera_c=mat_mult(mat_mult(rotation_mat,translation_mat),world_c);

    return camera_c;
}


vector<double> scene::camera_to_world(vector<double> camera_c)
{
    vector<double> world_c(camera_c.size(),0);
    world_c=mat_mult(mat_mult(inv_translation_mat,inv_rotation_mat),camera_c);

    return world_c;
}