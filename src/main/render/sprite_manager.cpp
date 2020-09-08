#include <render/sprite_manager.hpp>

#include <memory>
#include <map>
#include <vml/mat3.hpp>
#include <vector>
#include <render/render_manager.hpp>
#include <resource/resource_manager.hpp>

namespace render::sprite_manager {
    namespace {
        struct info {
            std::map<std::string, uint32_t> name_id_map;
            std::vector<vml::mat3> transform_list;

            uint32_t unknown_id;
            vml::mat3 unknown_t;

            float width;
            float height;
            float layers;
        };
        std::unique_ptr<info> info_p;

        uint32_t convert_endian(const uint8_t* in) {
            uint32_t out = 0;
            out += ((uint32_t)in[0]) << 24;
            out += ((uint32_t)in[1]) << 16;
            out += ((uint32_t)in[2]) << 8;
            out += ((uint32_t)in[3]) << 0;
            return out;
        }
        vml::mat3 construct_transform(const uint8_t* data) {
            return vml::mat3(
                    convert_endian(data) / info_p->width, 0.0f, 0.0f,
                    0.0f, convert_endian(data + 4) / info_p->height, 0.0f,
                    convert_endian(data + 8) / info_p->width, convert_endian(data + 12) / info_p->height, convert_endian(data + 16) / info_p->layers);
        }
    }
    bool init() {
        info_p = std::make_unique<info>();

        std::vector<uint8_t> description_data = resource::resource_manager::read_binary_file("sprites.ats", {"textures"});
        if (description_data.size() < 4) {
            info_p.reset(nullptr);
            return false;
        }
        auto it = description_data.begin();

        uint32_t sprite_count = convert_endian(&*it);
        it += 4;

        for (uint32_t i = 0; i < sprite_count; i++) {
            if (description_data.end() - it < 20) {
                info_p.reset(nullptr);
                return false;
            }
            info_p->transform_list.push_back(construct_transform(&*it));
            it += 20;
            std::string name;
            while (*it != 0 && it != description_data.end()) {
                name += *(char*)(&*it);
                it += 1;
            }
            if (*it != 0) {
                info_p.reset(nullptr);
                return false;
            }
            info_p->name_id_map.insert(std::pair<std::string, uint32_t>(name, i + 1));
            it += 1;
        }
        info_p->unknown_id = get_sprite("unknown");
        if (info_p->unknown_id == 0) {
            info_p.reset(nullptr);
            return false;
        }
        info_p->unknown_t = info_p->transform_list.at(info_p->unknown_id - 1);
        return true;
    }
    uint32_t get_sprite(const std::string& name) {
        auto it = info_p->name_id_map.find(name);
        if (it != info_p->name_id_map.end()) {
            return it->second;
        }
        return 0;
    }
    void bind_sprite(uint32_t id) {
        vml::mat3 transform = info_p->unknown_t;
        if (id > 0) {
            if (id <= info_p->transform_list.size()) {
                transform = info_p->transform_list.at(id - 1);
            }
        }
        render_manager::set_texture_transform(transform);
    }
}