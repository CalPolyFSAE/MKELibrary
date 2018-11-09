#include <Service.h>
#include <stdint.h>
#include <Event.h>

namespace BSP {
namespace I2C {
using callback_type = se::Event<void(uint8_t data[], uint8_t length)>;
struct I2Cconfig {
	uint32_t baudRate;
};

class I2C: public StaticService<I2C, const I2Cconfig&> {
public:
	I2C(const I2Cconfig&);

	void tick() override;
	void init() override;
	void tx(uint8_t address, uint8_t data[], uint8_t length);
	void rx(callback_type& callback);

private:
	I2Cconfig config;
};

}
}
