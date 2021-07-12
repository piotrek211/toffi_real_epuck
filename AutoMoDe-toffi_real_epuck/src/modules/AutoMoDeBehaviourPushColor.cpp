/**
  * @file <src/modules/AutoMoDeBehaviourAttractionColor.cpp>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#include "AutoMoDeBehaviourPushColor.h"


namespace argos {

	/****************************************/
	/****************************************/

    AutoMoDeBehaviourPushColor::AutoMoDeBehaviourPushColor() {
        m_strLabel = "PushColor";
	}

	/****************************************/
	/****************************************/

    AutoMoDeBehaviourPushColor::AutoMoDeBehaviourPushColor(AutoMoDeBehaviourPushColor* pc_behaviour) {
		m_strLabel = pc_behaviour->GetLabel();
		m_bLocked = pc_behaviour->IsLocked();
		m_bOperational = pc_behaviour->IsOperational();
		m_unIndex = pc_behaviour->GetIndex();
		m_unIdentifier = pc_behaviour->GetIdentifier();
		m_mapParameters = pc_behaviour->GetParameters();
		Init();
	}

	/****************************************/
	/****************************************/

    AutoMoDeBehaviourPushColor::~AutoMoDeBehaviourPushColor() {}

	/****************************************/
	/****************************************/

    AutoMoDeBehaviourPushColor* AutoMoDeBehaviourPushColor::Clone() {
        return new AutoMoDeBehaviourPushColor(this);   // todo: check without *
	}

	/****************************************/
	/****************************************/

    void AutoMoDeBehaviourPushColor::ControlStep() {
		//std::cout << "PUSH COLOR: " << m_cColorReceiverParameter << std::endl;
        CCI_EPuckOmnidirectionalCameraSensor::SReadings sReadings = m_pcRobotDAO->GetCameraInput();
        CCI_EPuckOmnidirectionalCameraSensor::TBlobList::iterator it;
        CVector2 sColVectorSum(0,CRadians::ZERO);
        CVector2 sProxVectorSum(0,CRadians::ZERO);
		CVector2 sResultVector(0,CRadians::ZERO);
        for (it = sReadings.BlobList.begin(); it != sReadings.BlobList.end(); it++) {
	    if ((*it)->Color == m_cColorReceiverParameter && (*it)->Distance <= 17.0f) {
                sColVectorSum += CVector2(1 / (((*it)->Distance)+1), (*it)->Angle);
            }
        }
	   m_pcRobotDAO->SetWheelsVelocity(0, 0);
       if(sColVectorSum.SquareLength() == 0){
            sProxVectorSum = CVector2(m_pcRobotDAO->GetProximityReading().Value, m_pcRobotDAO->GetProximityReading().Angle);
			m_eGoDirection = AVOID;
        } else {
			Real offset = 0.1;
			sResultVector = CVector2(m_unAttractionParameter, sColVectorSum.Angle().SignedNormalize());
			if (sColVectorSum.Angle().SignedNormalize() < (CRadians(0.0f) - CRadians(offset))) {
				m_eGoDirection = RIGHT;
			} else if(sColVectorSum.Angle().SignedNormalize() > (CRadians(0.0f) + CRadians(offset))) {
				m_eGoDirection = LEFT;
			} else {
				m_eGoDirection = STRAIGHT;
			}
		}
		Real velocity = 0.5f;
		switch (m_eGoDirection) {
			case RIGHT: {
				m_pcRobotDAO->SetWheelsVelocity(velocity, -velocity);
				break;
			}
			case LEFT: {
				m_pcRobotDAO->SetWheelsVelocity(-velocity, velocity);
				break;
			}
			case STRAIGHT: {
				m_pcRobotDAO->SetWheelsVelocity(ComputeWheelsVelocity(1, 1));
				break;
			}
			case AVOID: {
				sResultVector = CVector2(m_unAttractionParameter, sColVectorSum.Angle().SignedNormalize()) - 6*sProxVectorSum;
				m_pcRobotDAO->SetWheelsVelocity(ComputeWheelsVelocityFromVector(sResultVector));
				break;
			}
		}


		m_bLocked = false;
	}

	/****************************************/
	/****************************************/

    void AutoMoDeBehaviourPushColor::Init() {
        std::map<std::string, Real>::iterator it = m_mapParameters.find("vel");
		if (it != m_mapParameters.end()) {
			m_unAttractionParameter = it->second;
		} else {
			LOGERR << "[FATAL] Missing parameter for the following behaviour:" << m_strLabel << std::endl;
			THROW_ARGOSEXCEPTION("Missing Parameter");
		}
        it = m_mapParameters.find("clr");
        if (it != m_mapParameters.end()) {
            m_cColorReceiverParameter = GetColorParameter(it->second, false);
        } else {
            LOGERR << "[FATAL] Missing parameter for the following behaviour:" << m_strLabel << std::endl;
            THROW_ARGOSEXCEPTION("Missing Parameter");
        }
	}

	/****************************************/
	/****************************************/

    void AutoMoDeBehaviourPushColor::Reset() {
		m_bOperational = false;
		ResumeStep();
	}

	/****************************************/
	/****************************************/

    void AutoMoDeBehaviourPushColor::ResumeStep() {
		m_bOperational = true;
	}
}

