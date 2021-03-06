/** ***************************************************************************
 * @file extern_port.h functions for general external port interface
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * @brief UCB (Unified Code Base) external serial interface
 *        external communication port structures
 *****************************************************************************/
/*******************************************************************************
Copyright 2020 ACEINNA, INC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#ifndef EXTERN_PORT_H
#define EXTERN_PORT_H

#include <stdint.h>
#include "constants.h"
#include "ucb_packet.h"

extern BOOL     HandleUcbRx (UcbPacketStruct *ptrUcbPacket);
extern void     HandleUcbTx (int port, UcbPacketStruct *ptrUcbPacket);

#endif
