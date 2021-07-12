/**
  * @file <src/modules/AutoMoDeBehaviourAttractionColor.h>
  *
  * @author Antoine Ligot - <aligot@ulb.ac.be>
  *
  * @package ARGoS3-AutoMoDe
  *
  * @license MIT License
  */

#ifndef AUTOMODE_BEHAVIOUR_PUSH_COLOR_H
#define AUTOMODE_BEHAVIOUR_PUSH_COLOR_H

#include "AutoMoDeBehaviour.h"

namespace argos {
    class AutoMoDeBehaviourPushColor: public AutoMoDeBehaviour{
		public:
            AutoMoDeBehaviourPushColor();
            AutoMoDeBehaviourPushColor(AutoMoDeBehaviourPushColor* pc_behaviour);
            virtual ~AutoMoDeBehaviourPushColor();

			virtual void ControlStep();
			virtual void Reset();
			virtual void ResumeStep();
			virtual void Init();

            virtual AutoMoDeBehaviourPushColor* Clone();

        private:
            enum GoDirection {
				        LEFT,
				        RIGHT,
                STRAIGHT,
                AVOID
			      };


            GoDirection m_eGoDirection;
            Real m_fDistanceWeightParameter;
            CColor m_cColorEmiterParameter;
            CColor m_cColorReceiverParameter;
            Real m_unAttractionParameter;
	};
}

#endif

