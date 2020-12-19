#include"primitive.h"
#include<sstream>
#include<cstdio>
#include<string>
#include<cmath>
#include<iostream>
#include<cstdlib>
#include<algorithm>
#define ran() ( double( rand() % 32768 ) / 32768 )

const int BEZIER_MAX_DEGREE = 5;
const int Combination[BEZIER_MAX_DEGREE + 1][BEZIER_MAX_DEGREE + 1] =
{	0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0,
	1, 2, 1, 0, 0, 0,
	1, 3, 3, 1, 0, 0,
	1, 4, 6, 4, 1, 0,
	1, 5, 10,10,5, 1
};

const int MAX_COLLIDE_TIMES = 10;
const int MAX_COLLIDE_RANDS = 10;


std::pair<double, double> ExpBlur::GetXY()
{
	double x,y;
	x = ran();
	x = pow(2, x)-1;
	y = rand();
	return std::pair<double, double>(x*cos(y),x*sin(y));
}

Material::Material() {
	color = absor = Color();
	refl = refr = 0;
	diff = spec = 0;
	rindex = 0;
	drefl = 0;
	texture = NULL;
	blur = new ExpBlur();
}

void Material::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
	if ( var == "absor=" ) absor.Input( fin );
	if ( var == "refl=" ) fin >> refl;
	if ( var == "refr=" ) fin >> refr;
	if ( var == "diff=" ) fin >> diff;
	if ( var == "spec=" ) fin >> spec;
	if ( var == "drefl=" ) fin >> drefl;
	if ( var == "rindex=" ) fin >> rindex;
	if ( var == "texture=" ) {
		std::string file; fin >> file;
		texture = new Bmp;
		texture->Input( file );
	}
	if ( var == "blur=" ) {
		std::string blurname; fin >> blurname;
		if(blurname == "exp")
			blur = new ExpBlur();
	}
}

Primitive::Primitive() {
	sample = rand();
	material = new Material;
	next = NULL;
}

Primitive::Primitive( const Primitive& primitive ) {
	*this = primitive;
	material = new Material;
	*material = *primitive.material;
}

Primitive::~Primitive() {
	delete material;
}

void Primitive::Input( std::string var , std::stringstream& fin ) {
	material->Input( var , fin );
}

Sphere::Sphere() : Primitive() {
	De = Vector3( 0 , 0 , 1 );
	Dc = Vector3( 0 , 1 , 0 );
}

void Sphere::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "De=" ) De.Input( fin );
	if ( var == "Dc=" ) Dc.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Sphere::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 P = ray_O - O;
	double b = -P.Dot( ray_V );
	double det = b * b - P.Module2() + R * R;
	CollidePrimitive ret;

	if ( det > EPS ) {
		det = sqrt( det );
		double x1 = b - det  , x2 = b + det;

		if ( x2 < EPS ) return ret;
		if ( x1 > EPS ) {
			ret.dist = x1;
			ret.front = true;
		} else {
			ret.dist = x2;
			ret.front = false;
		} 
	} else 
		return ret;

	ret.C = ray_O + ray_V * ret.dist;
	ret.N = ( ret.C - O ).GetUnitVector();
	if ( ret.front == false ) ret.N = -ret.N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Sphere::GetTexture(Vector3 crash_C) {
	Vector3 I = ( crash_C - O ).GetUnitVector();
	double a = acos( -I.Dot( De ) );
	double b = acos( std::min( std::max( I.Dot( Dc ) / sin( a ) , -1.0 ) , 1.0 ) );
	double u = a / PI , v = b / 2 / PI;
	if ( I.Dot( Dc * De ) < 0 ) v = 1 - v;
	return material->texture->GetSmoothColor( u , v );
}


void Plane::Input( std::string var , std::stringstream& fin ) {
	if ( var == "N=" ) N.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Plane::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	N = N.GetUnitVector();
	double d = N.Dot( ray_V );
	CollidePrimitive ret;
	if ( fabs( d ) < EPS ) return ret;
	double l = ( N * R - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return ret;

	ret.dist = l;
	ret.front = ( d < 0 );
	ret.C = ray_O + ray_V * ret.dist;
	ret.N = ( ret.front ) ? N : -N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Plane::GetTexture(Vector3 crash_C) {
	double u = crash_C.Dot( Dx ) / Dx.Module2();
	double v = crash_C.Dot( Dy ) / Dy.Module2();
	return material->texture->GetSmoothColor( u , v );
}

void Square::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Primitive::Input( var , fin );
}

CollidePrimitive Square::Collide( Vector3 ray_O , Vector3 ray_V ) {
	//NEED TO IMPLEMENT
	CollidePrimitive ret;
	ray_V = ray_V.GetUnitVector();
	Vector3 N = (Dx * Dy).GetUnitVector();
	double d = N.Dot(ray_V);
	if (fabs(d) < EPS) 
		return ret;
	double l = (O - ray_O).Dot(N) / d;
	if (l < EPS) 
		return ret;

	Vector3 P = ray_O + ray_V * l;
	Vector3 OP = P - O;

	double px = OP.Dot(Dx.GetUnitVector());
	double py = OP.Dot(Dy.GetUnitVector());
	double dx = Dx.Module();
	double dy = Dy.Module();

	if ((px > (dx + EPS)) || (px < -(dx + EPS))) {
		return ret;
	}
	if ((py > (dy + EPS)) || (py < -(dy + EPS))) {
		return ret;
	}

	ret.dist = l;
	ret.front = (d < 0);
	ret.C = ray_O + ray_V * ret.dist;
	ret.N = (ret.front) ? N : -N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Square::GetTexture(Vector3 crash_C) {
	double u = (crash_C - O).Dot( Dx ) / Dx.Module2() / 2 + 0.5;
	double v = (crash_C - O).Dot( Dy ) / Dy.Module2() / 2 + 0.5;
	return material->texture->GetSmoothColor( u , v );
}


void Cube::Input(std::string var, std::stringstream& fin) {
	if (var == "O=") O.Input(fin);
	if (var == "Dx=") Dx.Input(fin);
	if (var == "Dy=") Dy.Input(fin);
	if (var == "x=") fin >> x;
	if (var == "y=") fin >> y;
	if (var == "z=") fin >> z;
	Primitive::Input(var, fin);
}

CollidePrimitive Cube::Collide(Vector3 ray_O, Vector3 ray_V) {
	//NEED TO IMPLEMENT
	
	ray_V = ray_V.GetUnitVector();
	Dx = Dx.GetUnitVector() * x;
	Dy = Dy.GetUnitVector() * y;

	CollidePrimitive ret;
	CollidePrimitive ret1;
	CollidePrimitive ret2;

	for (int i = 0; i < 6; i++) {
		// 0 前面
		// 1 左面
		// 2 后面
		// 3 右面
		// 4 上面
		// 5 下面
		Vector3 Dz = ((Dx * Dy).GetUnitVector()) * z;
		Vector3 Ns, Os;
		switch (i) {
			case 0:
				Ns = Dz.GetUnitVector();
				Os = O + Dz;
				break;
			case 1:
				Ns = -Dx.GetUnitVector();
				Os = O - Dx;
				break;
			case 2:
				Ns = -Dz.GetUnitVector();
				Os = O - Dz;
				break;
			case 3:
				Ns = Dx.GetUnitVector();
				Os = O + Dx;
				break;
			case 4:
				Ns = Dy.GetUnitVector();
				Os = O + Dy;
				break;
			case 5:
				Ns = -Dy.GetUnitVector();
				Os = O - Dy;
				break;
			default:
				std::cout << "something error!" << std::endl;
		}
		double d = Ns.Dot(ray_V);
		if (fabs(d) < EPS)
			continue;
		double l = (Os - ray_O).Dot(Ns) / d;
		if (l < EPS)
			continue;

		Vector3 P = ray_O + ray_V * l;
		Vector3 OP = P - Os;

		double px, py, dx, dy;

		switch (i) {
		case 0:
			px = OP.Dot(Dx.GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = Dx.Module();
			dy = Dy.Module();
			break;
		case 1:
			px = OP.Dot(Dz.GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = Dz.Module();
			dy = Dy.Module();
			break;
		case 2:
			px = OP.Dot((-Dx).GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = (-Dx).Module();
			dy = Dy.Module();
			break;
		case 3:
			px = OP.Dot((-Dz).GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = (-Dz).Module();
			dy = Dy.Module();
			break;
		case 4:
			px = OP.Dot(Dx.GetUnitVector());
			py = OP.Dot((-Dz).GetUnitVector());
			dx = Dx.Module();
			dy = (-Dz).Module();
			break;
		case 5:
			px = OP.Dot(Dx.GetUnitVector());
			py = OP.Dot(Dz.GetUnitVector());
			dx = Dx.Module();
			dy = Dz.Module();
			break;
		default:
			std::cout << "something error2!" << std::endl;
		}

		/*switch (i) {
		case 0:
			px = OP.Dot(Dx.GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = x;
			dy = y;
			break;
		case 1:
			px = OP.Dot(Dz.GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = z;
			dy = y;
			break;
		case 2:
			px = OP.Dot((-Dx).GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = x;
			dy = y;
			break;
		case 3:
			px = OP.Dot((-Dz).GetUnitVector());
			py = OP.Dot(Dy.GetUnitVector());
			dx = z;
			dy = y;
			break;
		case 4:
			px = OP.Dot(Dx.GetUnitVector());
			py = OP.Dot((-Dz).GetUnitVector());
			dx = x;
			dy = z;
			break;
		case 5:
			px = OP.Dot(Dx.GetUnitVector());
			py = OP.Dot(Dz.GetUnitVector());
			dx = x;
			dy = z;
			break;
		default:
			std::cout << "something error2!" << std::endl;
		}*/


		if ((px > (dx + EPS)) || (px < -(dx + EPS))) {
			continue;
		}
		if ((py > (dy + EPS)) || (py < -(dy + EPS))) {
			continue;
		}

		if (ret1.isCollide == false) {
			ret1.dist = l;
			ret1.front = (d < 0);
			ret1.C = ray_O + ray_V * ret1.dist;
			ret1.N = (ret1.front) ? Ns : -Ns;
			ret1.isCollide = true;
			ret1.collide_primitive = this;
		}
		else {
			ret2.dist = l;
			ret2.front = (d < 0);
			ret2.C = ray_O + ray_V * ret2.dist;
			ret2.N = (ret2.front) ? Ns : -Ns;
			ret2.isCollide = true;
			ret2.collide_primitive = this;
			break;
		}
	}

	// 是否找到了两个交点
	if (ret1.isCollide == true && ret2.isCollide == true) {
		//std::cout << ret1.front<<" "<<ret2.front << std::endl;
		/*if (ret1.front == true) {
			ret.dist = ret1.dist;
			ret.front = ret1.front;
			ret.C = ret1.C;
			ret.N = ret1.N;
			ret.isCollide = true;
			ret.collide_primitive = this;
			return ret;
		}
		else {
			ret.dist = ret2.dist;
			ret.front = ret2.front;
			ret.C = ret2.C;
			ret.N = ret2.N;
			ret.isCollide = true;
			ret.collide_primitive = this;
			return ret;
		}*/
		if (ret1.dist < ret2.dist) {
			ret.dist = ret1.dist;
			ret.front = ret1.front;
			ret.C = ret1.C;
			ret.N = ret1.N;
			ret.isCollide = true;
			ret.collide_primitive = this;
			return ret;
		}
		else {
			ret.dist = ret2.dist;
			ret.front = ret2.front;
			ret.C = ret2.C;
			ret.N = ret2.N;
			ret.isCollide = true;
			ret.collide_primitive = this;
			return ret;
		}
	}
	else if (ret1.isCollide == true) {
		return ret1;
	}
	
	return ret;
}

Color Cube::GetTexture(Vector3 crash_C) {
	Dx = Dx.GetUnitVector() * x;
	Dy = Dy.GetUnitVector() * y;
	Vector3 X, Y;

	for (int i = 0; i < 6; i++) {
		Vector3 Dz = ((Dx * Dy).GetUnitVector()) * z;
		Vector3 Ns, Os;
		switch (i) {
		case 0:
			Ns = Dz.GetUnitVector();
			Os = O + Dz;
			break;
		case 1:
			Ns = -Dx.GetUnitVector();
			Os = O - Dx;
			break;
		case 2:
			Ns = -Dz.GetUnitVector();
			Os = O - Dz;
			break;
		case 3:
			Ns = Dx.GetUnitVector();
			Os = O + Dx;
			break;
		case 4:
			Ns = Dy.GetUnitVector();
			Os = O + Dy;
			break;
		case 5:
			Ns = -Dy.GetUnitVector();
			Os = O - Dy;
			break;
		default:
			std::cout << "something error!" << std::endl;
		}

		Vector3 OC = crash_C - Os;
		double l = OC.Dot(Ns);
		if (fabs(l) < EPS) {
			switch (i) {
			case 0:
				X = Dx;
				Y = Dy;
				break;
			case 1:
				X = Dz;
				Y = Dy;
				break;
			case 2:
				X = -Dx;
				Y = Dy;
				break;
			case 3:
				X = -Dz;
				Y = Dy;
				break;
			case 4:
				X = Dx;
				Y = -Dz;
				break;
			case 5:
				X = Dx;
				Y = Dz;
				break;
			default:
				std::cout << "something error!" << std::endl;
			}
		}

	}

	double u = (crash_C - O).Dot(X) / X.Module2() / 2 + 0.5;
	double v = (crash_C - O).Dot(Y) / Y.Module2() / 2 + 0.5;
	return material->texture->GetSmoothColor(u, v);
}




void Cylinder::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O1=" ) O1.Input( fin );
	if ( var == "O2=" ) O2.Input( fin );
	if ( var == "R=" ) fin>>R; 
	Primitive::Input( var , fin );
}

CollidePrimitive Cylinder::Collide( Vector3 ray_O , Vector3 ray_V ) {
	CollidePrimitive ret;
	//NEED TO IMPLEMENT
	Vector3 N2 = (O2 - O1).GetUnitVector();
	Vector3 N1 = (O1 - O2).GetUnitVector();
	Vector3 OD = ray_V * N2;
	Vector3 OC = (ray_O - O1) * N2;
	double det = OD.Dot(OC) * OD.Dot(OC) - OD.Module2() * (OC.Module2() - R * R);

	double dist;
	bool front;
	// 无解
	if (det > EPS) {
		det = sqrt(det);
		double x1 = (-OD.Dot(OC) - det) / OD.Module2();
		double x2 = (-OD.Dot(OC) + det) / OD.Module2();

		// 没有正数解
		if (x2 < EPS) 
			return ret;
		if (x1 > EPS) {
			dist = x1;
			front = true;
		}
		else {
			dist = x2;
			front = false;
		}
	}
	else
		return ret;

	Vector3 P = ray_O + ray_V * dist;
	double h = (P - O1).Dot(N2);
	Vector3 N = (P - O1 - h * N2).GetUnitVector();
	if (h < EPS)
	{
		double d1 = N1.Dot(ray_V);
		if (fabs(d1) < EPS) 
			return ret;
		double l1 = (O1 - ray_O).Dot(N1) / d1;
		if (l1 < EPS) 
			return ret;
		dist = l1;
		P = ray_O + ray_V * dist;
		N = N1;
		if (P.Distance(O1) > R - EPS) 
			return ret;
	}
	if (h > (O2 - O1).Module() - EPS)
	{
		double d2 = N2.Dot(ray_V);
		if (fabs(d2) < EPS) 
			return ret;
		double l2 = (O2 - ray_O).Dot(N2) / d2;
		if (l2 < EPS) 
			return ret;
		dist = l2;
		P = ray_O + ray_V * dist;
		N = N2;
		if (P.Distance(O2) > R - EPS) 
			return ret;
	}

	ret.dist = dist;
	ret.front = front;
	ret.C = P;
	ret.N = N;
	if (front == false) ret.N = -ret.N;
	ret.isCollide = true;
	ret.collide_primitive = this;
	return ret;
}

Color Cylinder::GetTexture(Vector3 crash_C) {
	double u = 0.5 ,v = 0.5;
	//NEED TO IMPLEMENT
	Vector3 N2 = (O2 - O1).GetUnitVector();

	if (fabs((crash_C - O1).Dot(N2)) < EPS ) {
		// 上下底面
		Vector3 vertical = N2.GetAnVerticalVector().GetUnitVector();
		Vector3 vertical2 = (vertical * N2).GetUnitVector();
		u = (crash_C - O1).Dot(vertical) / R;
		v = (crash_C - O1).Dot(vertical2) / R;
	}
	else if (fabs((crash_C - O2).Dot(N2)) < EPS) {
		// 上下底面
		Vector3 vertical = N2.GetAnVerticalVector().GetUnitVector();
		Vector3 vertical2 = (vertical * N2).GetUnitVector();
		u = (crash_C - O2).Dot(vertical) / R;
		v = (crash_C - O2).Dot(vertical2) / R;
	}
	else {
		u = (crash_C - O1).Dot(N2);
		Vector3 vertical = N2.GetAnVerticalVector();
		v = acos((crash_C - O1 - u * N2).GetUnitVector().Dot(vertical)) / PI / 2;
		if (vertical.Dot((crash_C - O1) * N2) < 0)
			v = 1 - v;
		u = u / (O2 - O1).Module();
	}
	return material->texture->GetSmoothColor( u , v );
}

void Bezier::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O1=" ) O1.Input( fin );
	if ( var == "O2=" ) O2.Input( fin );
	if ( var == "P=" ) {
		degree++;
		double newR, newZ;
		fin>>newZ>>newR;
		R.push_back(newR);
		Z.push_back(newZ);
	}
	if ( var == "Cylinder" ) {
		double maxR = 0;
		for(int i=0;i<R.size();i++)
			if(R[i] > maxR)
				maxR = R[i];
		boundingCylinder = new Cylinder(O1, O2, maxR);
		N = (O1 - O2).GetUnitVector();
		Nx = N.GetAnVerticalVector();
		Ny = N * Nx;
	}
	Primitive::Input( var , fin );
}

CollidePrimitive Bezier::Collide( Vector3 ray_O , Vector3 ray_V ) {
	CollidePrimitive ret;
	//NEED TO IMPLEMENT
	return ret;
}

Color Bezier::GetTexture(Vector3 crash_C) {
	double u = 0.5 ,v = 0.5;
	//NEED TO IMPLEMENT
	return material->texture->GetSmoothColor( u , v );
}

