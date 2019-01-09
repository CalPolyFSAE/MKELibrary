#ifndef LPIT_H_
#define LPIT_H_

#include "Service.h"
#include "fsl_lpit.h"

namespace BSP{

namespace lpit{

typedef void (*callback)(void); 

struct lpit_config {

    // i.e. kCLOCK_IpSrcSircAsync. Default will fail quietly
    clock_ip_src_t clock;

    // do or do not enable interrupts
    uint8_t interrupts = 1;

};

class LPIT final : public StaticService<LPIT, const lpit_config*> {
public:

    LPIT(const lpit_config*);

    void tick();

    void init(uint8_t ch, uint32_t period, callback cb);

    void interrupt(uint8_t ch);

private:

    LPIT() = default;

    callback callbacks[4] = {NULL};

    LPIT_Type* base = LPIT0;

};

}
}

#endif // LPIT_H_
