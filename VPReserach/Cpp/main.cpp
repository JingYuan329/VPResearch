#include<iostream>
#include"design.h"
using namespace std;

int main(int argc, char *argv[]) {
	design Design;
	cout << "start  " << endl;
	Design.parser_cell_lef(argv[1]);
	Design.parser_def(argv[2]);
	//Design.cout_cell_lef();
	//Design.cout_def();
	//Design.insert_VP_straight_initial("NAND4X1");


	/*Design.insert_all_VP_initial();
	Design.output_lef_file(argv[3]);*/

	//Design.insert_some_VP_initial(0.1);
	//Design.output_lef_file(argv[3]);



	//Design.make_rtree_M1();
	Design.make_PG_rtree_M3M4();
	Design.insert_dodge_VP(0.1);
	Design.output_dodge_lef_file(argv[3]);
	Design.output_dodge_def_file(argv[4]);

}