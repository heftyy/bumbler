#include "packet_structs.h"
#include "../actor/actor_ref/actor_ref.h"
#include "../messages/typed_message.h"

namespace bumbler {

void packet_data::load(std::unique_ptr<message> msg) {
    std::stringstream ss;
    boost::archive::text_oarchive archive(ss);
    archive << msg;
    data = ss.str();
}

}
