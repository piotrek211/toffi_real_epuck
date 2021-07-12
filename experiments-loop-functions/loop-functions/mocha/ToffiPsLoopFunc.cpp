/**
  * @file <loop-functions/IcraLoopFunc.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @license MIT License
  */

#include "ToffiPsLoopFunc.h"

/****************************************/
/****************************************/

ToffiPsLoopFunction::ToffiPsLoopFunction() {
    m_fObjectiveFunction = 0;
}

/****************************************/
/****************************************/

ToffiPsLoopFunction::ToffiPsLoopFunction(const ToffiPsLoopFunction& orig) {
}

/****************************************/
/****************************************/

ToffiPsLoopFunction::~ToffiPsLoopFunction() {}

/****************************************/
/****************************************/

void ToffiPsLoopFunction::Destroy() {

    m_tRobotStates.clear();
    RemoveArena();
}

/****************************************/
/****************************************/

void ToffiPsLoopFunction::Init(TConfigurationNode& t_tree) {

    TConfigurationNode cParametersNode;


    try {
      cParametersNode = GetNode(t_tree, "params");
      GetNodeAttributeOrDefault(cParametersNode, "build_arena", m_bBuildArena, (bool) false);
      GetNodeAttributeOrDefault(cParametersNode, "number_edges", m_unNumberEdges, (UInt32) 3);
      GetNodeAttributeOrDefault(cParametersNode, "number_boxes_per_edge", m_unNumberBoxes, (UInt32) 1);
      GetNodeAttributeOrDefault(cParametersNode, "length_boxes", m_fLengthBoxes, (Real) 0.25);
      GetNodeAttributeOrDefault(cParametersNode, "maximization", m_bMaximization, (bool) false);
      GetNodeAttributeOrDefault(cParametersNode, "dist_radius_epuck", m_fDistributionRadiusEpuck, (Real) 0.3);
    } catch(std::exception e) {
    }


    if (m_bBuildArena == true) {
        m_fArenaRadius = GetArenaRadious();
        m_fDistributionRadiusEpuck = m_fArenaRadius - m_fDistributionRadiusEpuck;
        m_fDistributionRadiusSmartObject = m_fArenaRadius - m_fDistributionRadiusSmartObject;
        PositionArena();
    }

    CoreLoopFunctions::Init(t_tree, false);

    InitRobotStates();

    m_pcArena->SetArenaColor(CColor::BLACK);

}

/****************************************/
/****************************************/

void ToffiPsLoopFunction::Reset() {
    CoreLoopFunctions::Reset();

    m_fObjectiveFunction = 0;

    m_tRobotStates.clear();

    InitRobotStates();
}


/****************************************/
/****************************************/

void ToffiPsLoopFunction::PostStep() {

    m_unClock = GetSpace().GetSimulationClock();

    ScoreControl();
    ArenaControl();
}

/****************************************/
/****************************************/

void ToffiPsLoopFunction::PostExperiment() {
    if (m_bMaximization == true){
        LOG << -m_fObjectiveFunction << std::endl;
    }
    else {
        LOG << m_fObjectiveFunction << std::endl;
    }


}

/****************************************/
/****************************************/



UInt32 ToffiPsLoopFunction::GetPassageScore() {
    UpdateRobotPositions();
    UInt32 unScore = 0;
    TRobotStateMap::iterator it;

    for (it = m_tRobotStates.begin(); it != m_tRobotStates.end(); ++it) {
        if (it->second.cPosition.GetX() < 0.5) {  //0.62
            unScore++;
        }
    }
    return unScore;
}

/****************************************/
/****************************************/

Real ToffiPsLoopFunction::GetObjectiveFunction() {
    if (m_bMaximization == true){
        return -m_fObjectiveFunction;
    }
    else {
        return m_fObjectiveFunction;
    }
}


/****************************************/
/****************************************/

void ToffiPsLoopFunction::ArenaControl() {

    if (m_unClock == 1) {
        m_pcArena->SetWallColor(5, CColor::RED);
        m_pcArena->SetWallColor(7, CColor::RED);
        m_pcArena->SetWallColor(4, CColor::RED);
    }

    return;
}

/****************************************/
/****************************************/

void ToffiPsLoopFunction::ScoreControl(){
    m_fObjectiveFunction += GetPassageScore();
}
/****************************************/
/****************************************/

argos::CColor ToffiPsLoopFunction::GetFloorColor(const argos::CVector2& c_position_on_plane) {
    if(c_position_on_plane.GetX() > 0.0){
        return CColor::WHITE;
    }
    return CColor::GRAY50;
}

/****************************************/
/****************************************/

void ToffiPsLoopFunction::UpdateRobotPositions() {
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

void ToffiPsLoopFunction::InitRobotStates() {

    CSpace::TMapPerType& tEpuckMap = GetSpace().GetEntitiesByType("epuck");
    CVector2 cEpuckPosition(0,0);
    for (CSpace::TMapPerType::iterator it = tEpuckMap.begin(); it != tEpuckMap.end(); ++it) {
        CEPuckEntity* pcEpuck = any_cast<CEPuckEntity*>(it->second);
        cEpuckPosition.Set(pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetX(),
                           pcEpuck->GetEmbodiedEntity().GetOriginAnchor().Position.GetY());

        m_tRobotStates[pcEpuck].cLastPosition = cEpuckPosition;
        m_tRobotStates[pcEpuck].cPosition = cEpuckPosition;
        std::cout << cEpuckPosition << std::endl;
    }
}

/****************************************/
/****************************************/

CVector3 ToffiPsLoopFunction::GetRandomPosition(std::string m_sType) {
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
  Real fPosX = -0.35f + c*m_fDistributionRadius/2 * cos(2 * CRadians::PI.GetValue() * (a/b));
  Real fPosY = d*m_fDistributionRadius * sin(2 * CRadians::PI.GetValue() * (a/b));

  return CVector3(fPosX, fPosY, 0);
}

/****************************************/
/****************************************/

void ToffiPsLoopFunction::PositionArena() {
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

void ToffiPsLoopFunction::RemoveArena() {
    std::ostringstream id;
    id << "arena";
    RemoveEntity(id.str().c_str());
}

/****************************************/
/****************************************/

Real ToffiPsLoopFunction::GetArenaRadious() {

    Real fRadious;
    fRadious =  (m_fLengthBoxes*m_unNumberBoxes) / (2 * Tan(CRadians::PI / m_unNumberEdges));
    //fRadious = fRadious - 0.10; // Avoids to place robots close the walls.
    //fRadious = fRadious - 0.65; // Reduced cluster at the begining

    return fRadious;
}

/****************************************/
/****************************************/

bool ToffiPsLoopFunction::IsEven(UInt32 unNumber) {
    bool even;
    if((unNumber%2)==0)
       even = true;
    else
       even = false;

    return even;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(ToffiPsLoopFunction, "toffi_ps_loop_function");
