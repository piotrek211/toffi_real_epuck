/*
 * @file <src/CoreLoopFunctions.cpp>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 * @author Ken Hasselmann - <khasselm@ulb.ac.be>
 *
 * @package experiments-loop-functions
 *
 * @license MIT License
 */


#include "CoreLoopFunctions.h"

/****************************************/
/****************************************/

void CoreLoopFunctions::Init(argos::TConfigurationNode& t_tree, bool b_moveSmartObject) {
  m_pcRng = CRandom::CreateRNG("argos");
  b_moveSObject = b_moveSmartObject;
  TConfigurationNode cParametersNode;
  try {
    cParametersNode = GetNode(t_tree, "params");
    GetNodeAttributeOrDefault(cParametersNode, "number_robots", m_unNumberRobots, (UInt32) 1);
    GetNodeAttributeOrDefault(cParametersNode, "dist_radius_epuck", m_fDistributionRadiusEpuck, (Real) 0);
    GetNodeAttributeOrDefault(cParametersNode, "dist_radius_smart_object", m_fDistributionRadiusSmartObject, (Real) 0);
  } catch(std::exception e) {
    LOGERR << "Problem with Attributes in node params" << std::endl;
  }

  MoveRobots("epuck");
  if (b_moveSmartObject) {
    //MoveRobots("smart_object");
  }
  
}

/****************************************/
/****************************************/

void CoreLoopFunctions::Reset() {
  MoveRobots("epuck");
  if (b_moveSObject) {
    //MoveRobots("smart_object");
  }
}

/****************************************/
/****************************************/

CoreLoopFunctions::~CoreLoopFunctions() {}

/****************************************/
/****************************************/

void CoreLoopFunctions::MoveRobots(std::string m_sType) {
  CEmbodiedEntity* pcRobot;
  bool bPlaced = false;
  UInt32 unTrials;
  CSpace::TMapPerType& tRobotMap = GetSpace().GetEntitiesByType(m_sType);
  for (CSpace::TMapPerType::iterator it = tRobotMap.begin(); it != tRobotMap.end(); ++it) {
    if (m_sType == "epuck") {
      pcRobot = (CEmbodiedEntity*) &(any_cast<CEPuckEntity*>(it->second)->GetEmbodiedEntity());
    }
    unTrials = 0;
    do {
       ++unTrials;
       CVector3 cEpuckPosition = GetRandomPosition(m_sType);
       bPlaced = MoveEntity(*pcRobot,
                            cEpuckPosition,
                            CQuaternion().FromEulerAngles(m_pcRng->Uniform(CRange<CRadians>(CRadians::ZERO,CRadians::TWO_PI)),
                            CRadians::ZERO,CRadians::ZERO),false);
    } while(!bPlaced && unTrials < 1000);
    if(!bPlaced) {
       THROW_ARGOSEXCEPTION("Can't place robot");
    }
  }
}

/****************************************/
/****************************************/

void CoreLoopFunctions::RemoveRobots() {
  for(UInt32 i = 1; i < m_unNumberRobots + 1; ++i) {
    std::ostringstream id;
    id << "epuck" << i;
    RemoveEntity(id.str().c_str());
  }
}
