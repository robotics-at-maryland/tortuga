/*
 * Copyright (C) 2008  Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/CombineController.h
 */

#ifndef RAM_CONTROL_COMBINECONTROLLER_08_07_2008
#define RAM_CONTROL_COMBINECONTROLLER_08_07_2008

// Project Includes
#include "control/include/Common.h"
#include "control/include/ControllerBase.h"

#include "estimation/include/Common.h"

#include "vehicle/include/Common.h"

#include "core/include/ConfigNode.h"

#include "math/include/Vector2.h"
#include "math/include/Vector3.h"
#include "math/include/Quaternion.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

/** A class whichs allows easy combination of control algorithms
 *
 *  This class easily lets you change out just the rotational, or just the
 *  depth just the in plane controller.  Which allows for much easier over
 *  all experimentation with controllers.
 */

class RAM_EXPORT CombineController : public ControllerBase
{
public:
    /** Construct the controller with the given event hub, vehicle, and estimator.
     ** This constructor will mainly be used for testing purposes.
     **/
    CombineController(core::EventHubPtr eventHub,
                      vehicle::IVehiclePtr vehicle,
                      estimation::IStateEstimatorPtr estimator,
                      core::ConfigNode config);
    
    /** The controller finds its event hub, vehicle, and estimator from the 
     ** given list of subsystems 
     **/
    CombineController(core::ConfigNode config,
                      core::SubsystemList deps = core::SubsystemList());

    virtual ~CombineController();
    
    /** Returns a smart pointer to the current translational controller */
    ITranslationalControllerPtr getTranslationalController();
    
    /** Returns a smart pointer to the current depth controller */
    IDepthControllerPtr getDepthController();
    
    /** Returns a smart pointer to the current rotational controller */
    IRotationalControllerPtr getRotationalController();

protected:
    /* doUpdate - performs actual controller work
     *
     *  @param timestep
     *      The time sice the last update
     *  @param translationalForceOut
     *      The new force to apply to the vehicle (newtons)
     *  @param rotationalTorqueOut
     *      The new torque to apply to the vehicle (newtons)
     */
    virtual void doUpdate(const double& timestep,
                          math::Vector3& translationalForceOut,
                          math::Vector3& rotationalTorqueOut);

    
private:
    /** Does all initialzation based on the configuration settings */
    void init(core::ConfigNode config);

    /** Controller which handles the inplane vehicle motion */
    ITranslationalControllerImpPtr m_transController;
    
    /** Controller which holds the vehicle at the desired depth */
    IDepthControllerImpPtr m_depthController;
    
    /** Controller which holds the vehicle at the desired attitude */
    IRotationalControllerImpPtr m_rotController;

    /** Delay the start of the controller */
    double m_initializationPause;

    bool vConx;
    bool vCony;
    bool vConz;
    math::Vector2 intTermxy;
    double intTermz;
    double lastDV;
    double kpvx;
    double kdvx;
    double kivx;
    double kpvy;
    double kdvy;
    double kivy;
    double kpvz;
    double kdvz;
    double kivz;

};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_COMBINECONTROLLER_08_07_2008
