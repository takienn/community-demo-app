/****************************************************************************/
/// @file    application-logic.cpp
/// @author  Julen Maneros
/// @date    10.10.2010
///
// iTETRIS Cooperative ITS demo application related logic
/****************************************************************************/
// iTETRIS; see http://www.ict-itetris.eu/
// Copyright 2008-2010 iTETRIS consortium
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include "application-logic.h"
#include <time.h>

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include "utils/log/log.h"

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#include <unistd.h>
#endif

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

// ===========================================================================
// static member variables
// ===========================================================================
bool ApplicationLogic::m_camAreaIsSet = false;
bool ApplicationLogic::m_returnCarAreaIsSet = false;
vector<Vehicle> ApplicationLogic::m_vehiclesInArea;
vector<AppMessage> ApplicationLogic::m_messages;
int ApplicationLogic::m_messageCounter = 0;
Area ApplicationLogic::m_camArea;
Area ApplicationLogic::m_carArea;
int ApplicationLogic::m_appStartTimeStep;

// ===========================================================================
// method definitions
// ===========================================================================
Area
ApplicationLogic::GetCamArea()
{
    m_camAreaIsSet = true;
    return m_camArea;
}

Area
ApplicationLogic::GetReturningCarArea()
{
    m_returnCarAreaIsSet = true;
    return m_carArea;
}

bool
ApplicationLogic::DropSubscription()
{
    return false;
}

bool
ApplicationLogic::ProcessSubscriptionCarsInZone(vector<Vehicle>& vehicles)
{
    // Erase old data
    m_vehiclesInArea.clear();
    // Assign new data
    m_vehiclesInArea = vehicles;

    return true;
}

bool
ApplicationLogic::ProcessMessageNotifications(vector<AppMessage>& messages)
{
    // Show registered messages (logging purposes only)
    if (m_messages.size() == 0) {
        Log::Write("ProcessMessageNotifications() There are no registered messages", kLogLevelInfo);
        return true;
    } else {
        stringstream messagesOut;
        for (vector<AppMessage>::iterator it = m_messages.begin(); it != m_messages.end() ; it++) {
            AppMessage message = *it;
            if (it == m_messages.begin()) {
                messagesOut << "ProcessMessageNotifications() Registered Messages: " << message.messageId << ", ";
            } else if (it == messages.end()) {
                messagesOut << message.messageId;
            } else {
                messagesOut << message.messageId << ", ";
            }
        }
        Log::Write((messagesOut.str()).c_str(), kLogLevelInfo);
    }

    // Loop messages received by the iCS
    for (vector<AppMessage>::iterator it = messages.begin(); it != messages.end() ; ++it) {
        // Match message with the registered by the App
        for (vector<AppMessage>::iterator it_ = m_messages.begin(); it_ != m_messages.end() ; ++it_) {
            int inputId = (*it).messageId; // received from the iCS
            int registeredId = (*it_).messageId; // id registered by App
            if (inputId == registeredId) {
                (*it_).status = kToBeApplied;
            } else {
                stringstream log;
                log << "ProcessMessageNotifications() Message " << (*it).messageId << " not registered.";
                Log::Write((log.str()).c_str(), kLogLevelWarning);
            }
        }
    }

    return  true;
}

vector<AppMessage>
ApplicationLogic::SendBackExecutionResults(int senderId, int timestep)
{
    vector<AppMessage> results;

    if (timestep >= m_appStartTimeStep && senderId == 5000) {
        // Loop vehicles in the area, one message per vehicle
        for (vector<Vehicle>::const_iterator it = m_vehiclesInArea.begin() ; it != m_vehiclesInArea.end() ; it++) {
            AppMessage message;
            message.messageId = ++m_messageCounter;
            message.status = kToBeScheduled;
            message.senderId = senderId;
            message.destinationId = (*it).id;
            message.createdTimeStep = timestep;
            //message.payloadValue = (*it).speed + 10;
            message.payloadValue = 0;
            m_messages.push_back(message);
        }

        // Keep in safe place all the results to send back to the iCS
        results = m_messages;

        // Loop current messages to find those to be applied and erase from the result record
        for (vector<AppMessage>::iterator it = m_messages.begin() ; it != m_messages.end() ; it++) {
            if ((*it).status == kToBeApplied) {
                m_messages.erase(it); // The value will be applied so there is no need to have it registered any longer
            }
        }
    }

    return results;
}

int
ApplicationLogic::SetCamArea(float x, float y, float radius)
{
    if (radius <= 0) {
        Log::Write("Radius of the CAM area is 0 or negative.", kLogLevelError);
        return EXIT_FAILURE;
    }

    m_camArea.x = x;
    m_camArea.y = y;
    m_camArea.radius = radius;

    return EXIT_SUCCESS;
}

int
ApplicationLogic::SetCarArea(float x, float y, float radius)
{
    if (radius <= 0) {
        Log::Write("Radius of the returning car area is 0 or negative.", kLogLevelError);
        return EXIT_FAILURE;
    }

    m_carArea.x = x;
    m_carArea.y = y;
    m_carArea.radius = radius;

    return EXIT_SUCCESS;
}

int
ApplicationLogic::SetApplicationStartTimeStep(int timestep)
{
    if (timestep < 0) {
        Log::Write("Application start time step is negative", kLogLevelError);
        return EXIT_FAILURE;
    }

    m_appStartTimeStep = timestep;

    return EXIT_SUCCESS;
}