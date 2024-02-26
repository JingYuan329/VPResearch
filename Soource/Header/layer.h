#pragma once
#include<iostream>
#include<string>
using namespace std;
class layer
{
public:
	layer(string NAME, string DIR,string WIDTH,string SPACING) :
		_layer_name(NAME), _direction((DIR == "HORIZONTAL") ? 0 : 1), _width(stod(WIDTH)), _spacing(stod(SPACING)){}

	string get_layer_name() { return _layer_name; };
	int get_direction() { return _direction; };
	double get_width() { return _width; };
	double get_spacing() { return _spacing; };

private:
	string _layer_name;
	int _direction;
	double _width;	
	double _spacing;
};