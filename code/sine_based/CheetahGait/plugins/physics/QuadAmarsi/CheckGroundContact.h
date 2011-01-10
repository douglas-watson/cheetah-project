/**
 * \file CheckGroundContact2.h
 *
 *  \date Jun 28, 2010
 *  \author tuleu
 */

#ifndef LIBAMARSI_QUAD_CHECK_GROUND_CONTACT_H_
#define LIBAMARSI_QUAD_CHECK_GROUND_CONTACT_H_

#define _ODE_ODECPP_H_
#include <ode/ode.h>

namespace amarsi {
  /**
   * Checks the ground contact. It checks for a given body if its is in contact
   * with anoher body, through its operator() ().
   */
  class CheckGroundContact {
  public:
    /**
     * Checks if the given body is in contact with anontehr body.
     * If it is the case valid_ become false
     * @param b the dBody to check
     */
    void operator()(dBodyID b);
    /**
     * @return \c true if no contact has been made with the ground.
     */
    bool isValid() {
      return valid_;
    }
    /**
     * Initializes a new check.
     * @return
     */
    CheckGroundContact() {
      valid_ = true;
    }
    virtual ~CheckGroundContact() {
    }
  private :
    /**
     * \c true if no contact has been made.
     */
    static bool valid_;
  };
}

#endif // LIBAMARSI_QUAD_CHECK_GROUND_CONTACT_H_
