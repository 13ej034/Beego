#include <runCtrl.h>
#include <vutils.h>

int main(int argc, char** argv){

	RunCtrl run;
	run.connect("COM3");

	run.rotateAngle(VXV::deg(360));

	VXV::waitStable(run, 100);

	return 0;
}