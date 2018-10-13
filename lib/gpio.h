#include "Service.h"

class GPIO : public StaticService<GPIO> {
public:
	GPIO(){
		printf("GPIO INITIALIZED\n");
	}

	virtual void tick() override{
		printf("tock\n");
	}
};
