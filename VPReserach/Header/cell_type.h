#pragma once
#include"pin.h"
#include"cell.h"
using namespace std;
class cell_type
{
public:
	cell_type(string NAME,string SIZE,string BY) :
		_cell_type_name(NAME), _size(stod(SIZE)),_by(stod(BY)){}

	string get_cell_type_name() { return _cell_type_name; };
	double get_size() { return _size; };
	double get_by() { return _by; };
	pin* find_outpin() {
		for (int i = 0; i < _all_pin.size(); ++i) {
			if (_all_pin.at(i)->getdir() == "OUTPUT") return _all_pin.at(i);
		}
		return NULL; 
	};

	vector<pin*>_all_pin;
	vector<cell*>_this_type_all_cell;         //�ثe���ݭn�ϥ�map�A�������D����|���|�ݭn
	vector<pin*>_all_OBS;               // �H�e�˯u��obs�A�{�b�Τ���F
	bool _has_insert_VP=0;              //default ���O�S��insert VP��cell type

private:
	string _cell_type_name;
	double _size = 0;                    //_length
	double _by = 0;				// _width
	//ORIGIN
	//SYMMETRY

	//obs
};
