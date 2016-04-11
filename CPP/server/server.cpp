
// : astyle --style=google --indent=spaces --pad-header  --lineend=linux server.cpp
//
#include <iostream>
#include <fstream>
#include <string>

#include <ace/Arg_Shifter.h>

#include "MessengerS.h"

namespace {
class MessengerImpl : public virtual POA_Messenger {
  public:
    MessengerImpl() {}
    virtual ~MessengerImpl() {}

    virtual ::CORBA::Boolean string_op(
        const char * s_in,
        char *& s_inout,
        ::CORBA::String_out s_out);
};

::CORBA::Boolean MessengerImpl::string_op(
    const char * s_in,
    char *& s_inout,
    ::CORBA::String_out s_out) {
    // just print the input and in/output string
    std::cout << "Messenger::string_op() input: " << s_in << " - in/output: " << s_inout << std::endl;

    // create output string from in/output string
    s_out = ::CORBA::string_dup((std::string("this is the output string builded from in/output string: ") + s_inout).c_str());

    // reallocate in/output string from input string
    ::CORBA::string_free(s_inout);
    s_inout = ::CORBA::string_dup((std::string("this is new in/output string builded from input string: ") + s_in).c_str());

    return true;
}

void GetArgs(int argc, char *argv[], std::string &ior_file) {
    ACE_Arg_Shifter arg_shifter(argc, argv);
    while (arg_shifter.is_anything_left()) {
        const ACE_TCHAR *currentArg = 0;
        if ((currentArg = arg_shifter.get_the_parameter(ACE_TEXT("-ior"))) != 0) {
            ior_file = currentArg;
            arg_shifter.consume_arg();
        } else
            arg_shifter.ignore_arg();
    }
}
}

int main(int argc, char *argv[]) {
    try {
        // obtain arguments
        std::string ior_file;
        GetArgs(argc, argv, ior_file);

        // initialize the ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // obtain a reference to the RootPOA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        // activate the POAManager
        PortableServer::POAManager_var mgr = poa->the_POAManager();
        mgr->activate();

        // create the Messenger servant
        MessengerImpl servant;
        PortableServer::ObjectId_var oid = poa->activate_object(&servant);

        CORBA::Object_var messenger_obj = poa->id_to_reference(oid.in());

        // write the object reference to a file
        CORBA::String_var ior = orb->object_to_string(messenger_obj.in());
        std::ofstream out(ior_file.c_str());
        out << ior;
        out.close();

        // accept requests from clients
        orb->run();

        // cleanup
        orb->destroy();
    } catch (CORBA::Exception& ex) {
        std::cerr << "CORBA exception: " << ex << std::endl;
        return 1;
    }

    return 0;
}

