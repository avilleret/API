/*
// Copyright (c) 2016 Antoine Villeret
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include <m_pd.h> // puredata header
#include "ossia-pd.hpp"
#include "model.hpp"
#include "device.hpp"
#include "remote.hpp"
#include "parameter.hpp"

static t_class *ossia_class;

typedef struct t_ossia
{
    t_object    m_obj; // pd object - always placed in first in the object's struct

} t_ossia;

static void *ossia_new(t_symbol *name, int argc, t_atom *argv)
{
    t_ossia *x = (t_ossia *)pd_new(ossia_class);
    return (x);
}

static void ossia_free(t_ossia *x)
{
    ;
}

extern "C" void ossia_setup(void)
{
    t_class* c = class_new(gensym("ossia"),
                           (t_newmethod)ossia_new, nullptr,
                           sizeof(t_ossia), CLASS_DEFAULT, A_GIMME, 0);
    post("Welcome to ossia library");
    ossia_class = c;

    setup_ossia0x2emodel();
    setup_ossia0x2edevice();
    setup_ossia0x2eparam();
    setup_ossia0x2eremote();
}

// self registering (at loadbang or when creating the object)
template<typename T>
static bool obj_register(T *x)
{
    if (x->x_node) return true; // already registered

    std::cout << "obj_register: " << x->x_name->s_name << std::endl;

    int l;
    t_device *device = (t_device*) find_parent(&x->x_obj,osym_device, 0, &l);

    // first try to locate a ossia.device in the parent hierarchy...
    if (!device) {
        std::cerr << "no device at all, abording" << std::endl;
        return false; // not ready to register : if there is no device, model will be unable to register too
    }

    t_model *model = nullptr;
    t_view *view = nullptr;
    t_param *param = nullptr;
    int param_level, view_level, model_level;

    // then try to locate a view or a parameter when x is ossia.view or ossia.remote
    model = find_parent_alive<t_model>(&x->x_obj,osym_model, 0, &model_level);
    if (std::is_same<T,t_view>::value || std::is_same<T,t_remote>::value) {
        // param = (t_param*) find_parent(&x->x_obj, osym_param, 0, &param_level);
        // TODO : search param and test level agains view/model level
        view = find_parent_alive<t_view>(&x->x_obj,osym_view, 0, &view_level);
    }

    ossia::net::node_base*  node = nullptr;

    if (view){
        node = view->x_node;
    } else if (model){
        node = model->x_node;
    } else {
        node = device->x_node;
    }

    bool res = x->register_node(node);

    if ( !std::is_same<T,t_remote>::value ){
        std::cout << "remote_quarantine.size(): " << t_remote::remote_quarantine().size() << std::endl;
        for (auto remote : t_remote::remote_quarantine()){
            remote_loadbang(remote);
        }
    }

    return res;
}

template bool obj_register<t_param> (t_param *x);
template bool obj_register<t_remote>(t_remote *x);
template bool obj_register<t_model> (t_model *x);
template bool obj_register<t_view>  (t_view *x);