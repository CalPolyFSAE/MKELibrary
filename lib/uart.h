#ifndef UART_H_
#define UART_H_

#include "Service.h"
#include "fsl_lpuart.h"


namespace BSP {
namespace uart {

struct config {
	clock_ip_src_t clock = kCLOCK_IpSrcFircAsync;
};

class UART final : public StaticService<UART, const config*> {
public:
	void tick() override;

	UART(const config*);

	struct uartconfig {
		uint32_t baudrate = 115200U;
		uint8_t parity = 0;
		uint8_t stop = 1;
		uint8_t txen = 1;
		uint8_t rxen = 1;
	};

	void init(uint8_t no, uartconfig* conf);

private:

	static inline LPUART_Type* base(uint8_t no){
		switch(no){
		case 2:
			return LPUART2;
		case 1:
			return LPUART1;
		default:
			return LPUART0;
		}
	}

	clock_ip_src_t clock;
	clock_ip_name_t clockipnames[3] = {kCLOCK_Lpuart0, kCLOCK_Lpuart1, kCLOCK_Lpuart2};

};

}
}

#endif
