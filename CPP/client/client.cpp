
// : astyle --style=google --indent=spaces --pad-header  --lineend=linux client.cpp
// : ./CPP/client/client -ior my.ior -in hello -inout world
//
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <ace/Arg_Shifter.h>

#include "MessengerC.h"

void GetArgs(int argc, char *argv[], std::string &ior, char *&in_val, char *&inout_val) {
    ACE_Arg_Shifter arg_shifter(argc, argv);
    while (arg_shifter.is_anything_left()) {
        const ACE_TCHAR *currentArg = 0;

        if ((currentArg = arg_shifter.get_the_parameter(ACE_TEXT("-inout"))) != 0) {
            inout_val = ::CORBA::string_dup(currentArg);
            arg_shifter.consume_arg();

        } else if ((currentArg = arg_shifter.get_the_parameter(ACE_TEXT("-in"))) != 0) {
            in_val = ::CORBA::string_dup(currentArg);
            arg_shifter.consume_arg();

        } else if ((currentArg = arg_shifter.get_the_parameter(ACE_TEXT("-ior"))) != 0) {
            std::ifstream in(currentArg);
            if (!in)
                throw std::runtime_error((std::string("Cannot open: ") + currentArg).c_str());
            std::ostringstream ss;
            ss << in.rdbuf();
            if (!in && !in.eof())
                throw std::runtime_error((std::string("Cannot read IOR from: ") + currentArg).c_str());
            ior = ss.str();
            arg_shifter.consume_arg();

        } else
            arg_shifter.ignore_arg();
    }
}

int main(int argc, char *argv[]) {
    try {
        // obtain arguments
        std::string ior;
        char *in_val, *inout_val, *out_val;
        GetArgs(argc, argv, ior, in_val, inout_val);

        // initialize the ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // obtain an object reference
        CORBA::Object_var obj = orb->string_to_object(ior.c_str());
        Messenger_var messenger = Messenger::_narrow(obj.in());
        if (CORBA::is_nil(messenger.in()))
            throw std::runtime_error("IOR was not a Messenger object reference");

        // invoke the method
        messenger->string_op(in_val, inout_val, out_val);
        std::cout << "Messenger::string_op() input: " << in_val
                  << " - in/output: " << inout_val
                  << " - output: " << out_val << std::endl;

        ::CORBA::string_free(in_val);
        ::CORBA::string_free(inout_val);
        ::CORBA::string_free(out_val);

        // cleanup
        orb->destroy();
    } catch (CORBA::Exception& ex) {
        std::cerr << "CORBA exception: " << ex << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

