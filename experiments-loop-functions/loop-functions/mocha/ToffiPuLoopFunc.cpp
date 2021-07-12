/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "ToffiPuLoopFunc.h"

/****************************************/
/****************************************/

ToffiPuLoopFunction::ToffiPuLoopFunction() {
    m_unClock = 0;
    m_unStopTime = 0;
    m_unStopEdge = 2;
    m_unStopBox = 2;
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

ToffiPuLoopFunction::ToffiPuLoopFunction(const ToffiPuLoopFunction& orig) {
}

/****************************************/
/****************************************/

ToffiPuLoopFunction::~ToffiPuLoopFunction() {}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::Destroy() {
    m_tRobotStates.clear();
    RemoveArena();
}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::Init(TConfigurationNode& t_tree) {

    TConfigurationNode cParametersNode;


    try {
      cParametersNode = GetNode(t_tree, "params");
      GetNodeAttributeOrDefault(cParametersNode, "build_arena", m_bBuildArena, (bool) false);
      GetNodeAttributeOrDefault(cParametersNode, "number_edges", m_unNumberEdges, (UInt32) 3);
      GetNodeAttributeOrDefault(cParametersNode, "number_boxes_per_edge", m_unNumberBoxes, (UInt32) 1);
      GetNodeAttributeOrDefault(cParametersNode, "length_boxes", m_fLengthBoxes, (Real) 0.25);
      GetNodeAttributeOrDefault(cParametersNode, "maximization", m_bMaximization, (bool) false);
      GetNodeAttributeOrDefault(cParametersNode, "wall_color", m_cWallColor, (UInt32) 0);
      GetNodeAttributeOrDefault(cParametersNode, "dist_radius_epuck", m_fDistributionRadiusEpuck, (Real) 0.3);
      GetNodeAttributeOrDefault(cParametersNode, "dist_radius_smart_object", m_fDistributionRadiusSmartObject, (Real) 0.4);
    } catch(std::exception e) {
    }


    if (m_bBuildArena == true) {
        m_fArenaRadius = GetArenaRadious();
        m_fDistributionRadiusEpuck = m_fArenaRadius - m_fDistributionRadiusEpuck;
        PositionArena();
    }

    CoreLoopFunctions::Init(t_tree);

    InitRobotStates();

    m_pcArena->SetArenaColor(CColor::BLACK);

}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::SetWallColor() {
    CColor wall_color;

    switch (m_cWallColor) {
    case 1:
        wall_color = CColor::MAGENTA;
        cFinalColor = CColor::RED;
        break;
    case 2:
        wall_color = CColor::CYAN;
        cFinalColor = CColor::BLUE;
        break;
    case 3:
        wall_color = CColor::YELLOW;
        cFinalColor = CColor::GREEN;
        break;
    default:
        wall_color = CColor::BLACK;
        break;
    }
    m_pcArena->SetArenaColor(wall_color);

}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::Reset() {
    CoreLoopFunctions::Reset();

    m_pcArena->SetArenaColor(CColor::BLACK);
    m_unClock = 0;
    m_unStopEdge = 2;
    m_unStopBox = 2;
    m_unStopTime = 0;
    m_fObjectiveFunction = 0;

    m_tRobotStates.clear();

    InitRobotStates();
}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::PostStep() {
    m_unClock = GetSpace().GetSimulationClock();
}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::PostExperiment() {
    m_fObjectiveFunction = GetAggregationScore();
    if (m_bMaximization == true){
        LOG << -m_fObjectiveFunction << std::endl;
    }
    else {
        LOG << m_fObjectiveFunction << std::endl;
    }
}

/****************************************/
/****************************************/

Real ToffiPuLoopFunction::GetObjectiveFunction() {
    if (m_bMaximization == true){
        return -m_fObjectiveFunction;
    }
    else {
        return m_fObjectiveFunction;
    }
}

/****************************************/
/****************************************/


Real ToffiPuLoopFunction::GetAggregationScore() {

    return 0.0f;
}



/****************************************/
/****************************************/

argos::CColor ToffiPuLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
    Real border = 0.62;
    if(c_position_on_plane.GetX() < -border|| c_position_on_plane.GetX() > border ||
       c_position_on_plane.GetY() < -border || c_position_on_plane.GetY() > border){
        return CColor::WHITE;
    }
    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::UpdateRobotPositions() {
    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        m_tRobotStates[pcEpuck].cLastPosition = m_tRobotStates[pcEpuck].cPosition;
        m_tRobotStates[pcEpuck].cPosition = cEpuckPosition;
    }
}


/****************************************/
/****************************************/

void ToffiPuLoopFunction::InitRobotStates() {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        m_tRobotStates[pcEpuck].cLastPosition = cEpuckPosition;
        m_tRobotStates[pcEpuck].cPosition = cEpuckPosition;
        m_tRobotStates[pcEpuck].unItem = 0;
    }
}




/****************************************/
/****************************************/

CVector3 ToffiPuLoopFunction::GetRandomPosition(std::string m_sType) {
  m_fDistributionRadius = m_fDistributionRadiusEpuck;
  Real temp;
  Real a = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real b = m_pcRng->Uniform(CRange<Real>(0.0f, 1.0f));
  Real c = m_pcRng->Uniform(CRange<Real>(-1.0f, 1.0f));
  Real d = m_pcRng->Uniform(CRange<Real>(-1.0f, 1.0f));
  // If b < a, swap them
  if (b < a) {
    temp = a;
    a = b;
    b = temp;
  }
  //m_fDistributionRadius = m_fRadius;
  Real fPosX = (c * m_fDistributionRadius / 2) + m_fDistributionRadius * cos(2 * CRadians::PI.GetValue() * (a/b));
  Real fPosY = (d * m_fDistributionRadius / 2) + m_fDistributionRadius * sin(2 * CRadians::PI.GetValue() * (a/b));

  return CVector3(fPosX, fPosY, 0);
}

/****************************************/
/****************************************/

UInt32 ToffiPuLoopFunction::GetRandomTime(UInt32 unMin, UInt32 unMax) {
  UInt32 unStopAt = m_pcRng->Uniform(CRange<UInt32>(unMin, unMax));
  return unStopAt;

}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::PositionArena() {
  CArenaEntity* pcArena;
  /*
    pcArena = new CArenaEntity("arena",
                               CVector3(0,0,0),
                               CQuaternion().FromEulerAngles(CRadians::ZERO,CRadians::ZERO,CRadians::ZERO), // TODO
                               CVector3(0.01,m_fLenghtBoxes,0.1),
                               "leds",
                               m_unNumberBoxes,
                               m_unNumberEdges,
                               0.017f,
                               1.0f);   */ // arena with 12 leds per block

  pcArena = new CArenaEntity("arena",
                             CVector3(0,0,0),
                             CQuaternion().FromEulerAngles(CRadians::ZERO,CRadians::ZERO,CRadians::ZERO), // TODO
                             CVector3(0.01,m_fLengthBoxes,0.1),
                             "leds",
                             m_unNumberBoxes,
                             m_unNumberEdges,
                             0.125f,
                             1.0f);


 CWallEntity* wall_0 = new CWallEntity(pcArena, "wall_0",
                                     CVector3(0.7134,0.7134,0),
                             CQuaternion().FromEulerAngles(-3*CRadians::PI_OVER_FOUR,CRadians::ZERO,CRadians::ZERO), // TODO
                             CVector3(0.01,0.25,0.1),
                             "leds",
                             1,
                             0.125f,
                             1.0f);

   CWallEntity* wall_1 = new CWallEntity(pcArena, "wall_1",
                                     CVector3(-0.7134,0.7134,0),
                             CQuaternion().FromEulerAngles(-CRadians::PI_OVER_FOUR,CRadians::ZERO,CRadians::ZERO), // TODO
                             CVector3(0.01,0.25,0.1),
                             "leds",
                             1,
                             0.125f,
                             1.0f);

    CWallEntity* wall_2 = new CWallEntity(pcArena, "wall_2",
                                     CVector3(0.7134,-0.7134,0),
                             CQuaternion().FromEulerAngles(-5*CRadians::PI_OVER_FOUR,CRadians::ZERO,CRadians::ZERO), // TODO
                             CVector3(0.01,0.25,0.1),
                             "leds",
                             1,
                             0.125f,
                             1.0f);   

    CWallEntity* wall_3 = new CWallEntity(pcArena, "wall_3",
                                     CVector3(-0.7134,-0.7134,0),
                             CQuaternion().FromEulerAngles(CRadians::PI_OVER_FOUR,CRadians::ZERO,CRadians::ZERO), // TODO
                             CVector3(0.01,0.25,0.1),
                             "leds",
                             1,
                             0.125f,
                             1.0f);                      

     pcArena->AddWall(*wall_0);       
     pcArena->AddWall(*wall_1);  
     pcArena->AddWall(*wall_2);  
     pcArena->AddWall(*wall_3);  


  AddEntity(*pcArena);
  m_pcArena = pcArena;
}

/****************************************/
/****************************************/

void ToffiPuLoopFunction::RemoveArena() {
    std::ostringstream id;
    id << "arena";
    RemoveEntity(id.str().c_str());

}

/****************************************/
/****************************************/

Real ToffiPuLoopFunction::GetArenaRadious() {

    Real fRadious;
    fRadious =  (m_fLengthBoxes*m_unNumberBoxes) / (2 * Tan(CRadians::PI / m_unNumberEdges));
    //fRadious = fRadious - 0.15; // Avoids to place robots close the walls.
    //fRadious = fRadious - 0.65; // Reduced cluster at the begining

    return fRadious;
}

/****************************************/
/****************************************/

bool ToffiPuLoopFunction::IsEven(UInt32 unNumber) {
    bool even;
    if((unNumber%2)==0)
       even = true;
    else
       even = false;

    return even;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(ToffiPuLoopFunction, "toffi_pu_loop_function");
