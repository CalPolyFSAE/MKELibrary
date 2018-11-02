#include <Service.h>

namespace BSP {
namespace I2C {

struct I2Cconfig {
};

class I2C: public StaticService<I2C, const I2Cconfig&> {
public:
	I2C(const I2Cconfig&);

	void tick() override;
	void init() override;
};

}
}
