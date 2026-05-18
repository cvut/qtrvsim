#ifndef UTILANDTEXT_H
#define UTILANDTEXT_H

#include "machine/csr/address.h"
#include "machine/machinedefs.h"

const char *get_exception_name(machine::ExceptionCause exception_cause);
const char *get_privilege_level_name(machine::CSR::PrivilegeLevel level);

#endif // UTILANDTEXT_H
