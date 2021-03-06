/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "contact_listener.h"
#include "../physics/base_object.h"

#include "../scenes/level/lvl_player.h"
#include "../scenes/level/lvl_block.h"
#include "../scenes/level/lvl_bgo.h"
#include "../scenes/level/lvl_physenv.h"
#include "../scenes/level/lvl_warp.h"

#include <cstdint>

#include <QtDebug>

void PGEContactListener::BeginContact(b2Contact *contact)
{
    b2ContactListener::BeginContact(contact);

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Fixture* platformFixture = NULL;
    //b2Fixture* otherFixture = NULL;

    PGE_Phys_Object * bodyA = NULL;
    PGE_Phys_Object * bodyB = NULL;

    PGE_Phys_Object * bodyBlock = NULL;
    PGE_Phys_Object * bodyChar = NULL;

    bodyA = static_cast<PGE_Phys_Object *>(fixtureA->GetBody()->GetUserData());
    bodyB = static_cast<PGE_Phys_Object *>(fixtureB->GetBody()->GetUserData());

    if(!bodyA) return;
    if(!bodyB) return;

    /***********************************Warp & Player***********************************/
    if ( bodyA->type == PGE_Phys_Object::LVLWarp && bodyB->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( bodyB->type == PGE_Phys_Object::LVLWarp && bodyA->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        qDebug() <<"Contacted With Warp";
        dynamic_cast<LVL_Player *>(bodyChar)->contactedWarp = dynamic_cast<LVL_Warp *>(bodyBlock);
        dynamic_cast<LVL_Player *>(bodyChar)->contactedWithWarp=true;
        dynamic_cast<LVL_Player *>(bodyChar)->warpsTouched++;
    }

    /***********************************PhysEnvironment & Player***********************************/
    if ( bodyA->type == PGE_Phys_Object::LVLPhysEnv && bodyB->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( bodyB->type == PGE_Phys_Object::LVLPhysEnv && bodyA->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        dynamic_cast<LVL_Player *>(bodyChar)->environments_map[(intptr_t)bodyBlock]
                = dynamic_cast<LVL_PhysEnv *>(bodyBlock)->env_type;
    }

    /***********************************BGO & Player***********************************/
    if ( bodyA->type == PGE_Phys_Object::LVLBGO && bodyB->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( bodyB->type == PGE_Phys_Object::LVLBGO && bodyA->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        if(dynamic_cast<LVL_Bgo *>(bodyBlock)->setup->climbing)
            dynamic_cast<LVL_Player *>(bodyChar)->climbable_map[(intptr_t)bodyBlock]=1;
    }


    /***********************************Block & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLBlock) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLBlock) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        if(bodyBlock->collide==PGE_Phys_Object::COLLISION_ANY)
            if((bodyChar->bottom()<=bodyBlock->top()-0.1) ||
                    ((bodyChar->bottom() >= bodyBlock->top())&&
                    (bodyChar->bottom()<=bodyBlock->top()+2)))
            {
                dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map[(intptr_t)bodyBlock] = 1;
                dynamic_cast<LVL_Player *>(bodyChar)->onGround=(!dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map.isEmpty());
                if(dynamic_cast<LVL_Player *>(bodyChar)->keys.down)
                    dynamic_cast<LVL_Player *>(bodyChar)->climbing=false;
                if(bodyBlock->slippery_surface)
                    dynamic_cast<LVL_Player *>(bodyChar)->foot_sl_contacts_map[(intptr_t)bodyBlock] = 1;
            }
    }

}

void PGEContactListener::EndContact(b2Contact *contact)
{
    b2ContactListener::EndContact(contact);

    b2ContactListener::BeginContact(contact);

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Fixture* platformFixture = NULL;
    //b2Fixture* otherFixture = NULL;

    PGE_Phys_Object * bodyA = NULL;
    PGE_Phys_Object * bodyB = NULL;

    PGE_Phys_Object * bodyBlock = NULL;
    PGE_Phys_Object * bodyChar = NULL;

    bodyA = static_cast<PGE_Phys_Object *>(fixtureA->GetBody()->GetUserData());
    bodyB = static_cast<PGE_Phys_Object *>(fixtureB->GetBody()->GetUserData());

    if(!bodyA) return;
    if(!bodyB) return;

    Q_UNUSED(bodyBlock);

    /***********************************Warp & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLWarp) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        //bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLWarp) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        //bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        qDebug() <<"Contact With Warp end";
        dynamic_cast<LVL_Player *>(bodyChar)->warpsTouched--;
        if(dynamic_cast<LVL_Player *>(bodyChar)->warpsTouched==0)
        {
            dynamic_cast<LVL_Player *>(bodyChar)->contactedWithWarp=false;
            dynamic_cast<LVL_Player *>(bodyChar)->contactedWarp = NULL;
        }
    }
    /***********************************Physical Environment zone & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLPhysEnv) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLPhysEnv) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        if(dynamic_cast<LVL_Player *>(bodyChar)->environments_map.contains((intptr_t)bodyBlock))
        {
            dynamic_cast<LVL_Player *>(bodyChar)->environments_map.remove((intptr_t)bodyBlock);
        }
    }

    /***********************************BGO & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLBGO) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLBGO) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        if(dynamic_cast<LVL_Player *>(bodyChar)->climbable_map.contains((intptr_t)bodyBlock))
        {
            dynamic_cast<LVL_Player *>(bodyChar)->climbable_map.remove((intptr_t)bodyBlock);
            dynamic_cast<LVL_Player *>(bodyChar)->climbing =
                    ((!dynamic_cast<LVL_Player *>(bodyChar)->climbable_map.isEmpty()) &&
                        (dynamic_cast<LVL_Player *>(bodyChar)->climbing));
        }
    }

    /***********************************Block & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLBlock) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLBlock) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        if(dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map.contains((intptr_t)bodyBlock))
        {
            dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map.remove((intptr_t)bodyBlock);
            dynamic_cast<LVL_Player *>(bodyChar)->onGround  =
                    (!dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map.isEmpty());
        }

        if(dynamic_cast<LVL_Player *>(bodyChar)->foot_sl_contacts_map.contains((intptr_t)bodyBlock))
        {
            dynamic_cast<LVL_Player *>(bodyChar)->foot_sl_contacts_map.remove((intptr_t)bodyBlock);
        }
    }

}

void PGEContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
    b2ContactListener::PreSolve(contact, oldManifold);

        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();

        b2Fixture* platformFixture = NULL;
        //b2Fixture* otherFixture = NULL;

        PGE_Phys_Object * bodyA = NULL;
        PGE_Phys_Object * bodyB = NULL;

        PGE_Phys_Object * bodyBlock = NULL;
        PGE_Phys_Object * bodyChar = NULL;

        bodyA = static_cast<PGE_Phys_Object *>(fixtureA->GetBody()->GetUserData());
        bodyB = static_cast<PGE_Phys_Object *>(fixtureB->GetBody()->GetUserData());

        if(!bodyA) return;
        if(!bodyB) return;

        if ( bodyA->type == PGE_Phys_Object::LVLBlock && bodyB->type == PGE_Phys_Object::LVLPlayer )
        {
            platformFixture = fixtureA;
            //otherFixture = fixtureB;
            bodyBlock = bodyA;
            bodyChar = bodyB;
        }
        else if ( bodyB->type == PGE_Phys_Object::LVLBlock && bodyA->type == PGE_Phys_Object::LVLPlayer )
        {
            platformFixture = fixtureB;
            //otherFixture = fixtureA;
            bodyBlock = bodyB;
            bodyChar = bodyA;
        }
        else
        {
            platformFixture=NULL;
        }


        if(platformFixture)
        {
            if(bodyBlock->type == PGE_Phys_Object::LVLBlock)
            {
                if(dynamic_cast<LVL_Block *>(bodyBlock)->destroyed)
                {
                    contact->SetEnabled(false);
                    return;
                }

                if(dynamic_cast<LVL_Block *>(bodyBlock)->setup->lava)
                {
                    dynamic_cast<LVL_Player *>(bodyChar)->kill(LVL_Player::DEAD_burn);
                }
            }

            /*************************No collisions check*****************************/
            if ( bodyBlock->collide == PGE_Phys_Object::COLLISION_NONE )
            {
                contact->SetEnabled(false);
                return;
            }
            else
            /*************************Top collisions check*****************************/
            if( bodyBlock->collide == PGE_Phys_Object::COLLISION_TOP )
            {
                if(
                        (
                            (bodyChar->physBody->GetLinearVelocity().y > 0.1)
                            &&
                            (bodyChar->bottom() < bodyBlock->top()+1)
                            &&
                            (
                                 (bodyChar->left()<bodyBlock->right()-1 ) &&
                                 (bodyChar->right()>bodyBlock->left()+1 )
                             )
                         )
                        ||
                        (bodyChar->bottom() < bodyBlock->top())
                        )
                {
                    contact->SetEnabled(true);
                    dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map[(intptr_t)bodyBlock] = 1;
                    dynamic_cast<LVL_Player *>(bodyChar)->onGround  =
                            (!dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map.isEmpty());

                    if(dynamic_cast<LVL_Player *>(bodyChar)->keys.down)
                        dynamic_cast<LVL_Player *>(bodyChar)->climbing=false;
                    if(bodyBlock->slippery_surface)
                        dynamic_cast<LVL_Player *>(bodyChar)->foot_sl_contacts_map[(intptr_t)bodyBlock] = 1;

                }
                else //if( (bodyChar->bottom() > bodyBlock->top()+2) )
                {
                    contact->SetEnabled(false);
                }
                return;
            }
            else
            if( bodyBlock->collide == PGE_Phys_Object::COLLISION_ANY )
            /***********************Collision with any-side************************/
            {
                if(
                    ( (bodyChar->bottom() <= bodyBlock->top()-0.1)
                        ||
                      (
                            (bodyChar->bottom() >= bodyBlock->top())&&
                         (bodyChar->bottom()<=bodyBlock->top()+2)
                      )
                      )
                   )
                {
                    dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map[(intptr_t)bodyBlock]=1;
                    dynamic_cast<LVL_Player *>(bodyChar)->onGround  =
                            (!dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map.isEmpty());
                    if(dynamic_cast<LVL_Player *>(bodyChar)->keys.down)
                        dynamic_cast<LVL_Player *>(bodyChar)->climbing=false;

                    if(bodyBlock->slippery_surface)
                        dynamic_cast<LVL_Player *>(bodyChar)->foot_sl_contacts_map[(intptr_t)bodyBlock] = 1;
                }

                if(bodyChar->top() >= bodyBlock->bottom() && bodyChar->top() <= bodyBlock->bottom()+3
                        && (bodyChar->physBody->GetLinearVelocity().y < -0.01) )
                {
                    dynamic_cast<LVL_Player *>(bodyChar)->jumpForce=0;

                    if(dynamic_cast<LVL_Block *>(bodyBlock)->setup->hitable)
                    {
                        dynamic_cast<LVL_Player *>(bodyChar)->bump();
                    }
                    dynamic_cast<LVL_Block *>(bodyBlock)->hit();
                }

                if(dynamic_cast<LVL_Block *>(bodyBlock)->destroyed)
                {
                        dynamic_cast<LVL_Player *>(bodyChar)->bump();
                        contact->SetEnabled(false);
                        return;
                }

                if(dynamic_cast<LVL_Block *>(bodyBlock)->isHidden)
                {
                    contact->SetEnabled(false);
                    return;
                }

                if(bodyBlock->isRectangle)
                {
                    if( bodyChar->bottom() <= bodyBlock->top() && bodyChar->bottom() <= bodyBlock->top()+3 )
                    {
                        dynamic_cast<LVL_Player *>(bodyChar)->onGround=true;

                        if(dynamic_cast<LVL_Block *>(bodyBlock)->setup->bounce)
                        {
                            dynamic_cast<LVL_Player *>(bodyChar)->bump(true);
                            dynamic_cast<LVL_Block *>(bodyBlock)->hit(LVL_Block::down);
                        }
                    }
                    else
                    if( (bodyChar->bottom() > bodyBlock->top()) &&
                            (bodyChar->bottom() < bodyBlock->top()+2)
                           && (fabs(bodyChar->physBody->GetLinearVelocity().x)>0))
                    {
                        bodyChar->_player_moveup = true;
                        contact->SetEnabled(false);
                    }
                }
                else
                {
                    dynamic_cast<LVL_Player *>(bodyChar)->onGround=true;
                    dynamic_cast<LVL_Player *>(bodyChar)->foot_contacts_map[(intptr_t)bodyBlock]=1;
                }
                return;
            }

        }

}

void PGEContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *Impulse)
{
    b2ContactListener::PostSolve(contact, Impulse);

}
