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

#include "lvl_block.h"
#include "../../data_configs/config_manager.h"

#include "lvl_scene_ptr.h"

LVL_Block::LVL_Block()
{
    type = LVLBlock;
    f_block = NULL;
    f_edge  = NULL;

    animated=false;
    sizable=false;
    animator_ID=0;

    offset_x = 0.f;
    offset_y = 0.f;

    isHidden=false;
    destroyed=false;

    fadeOffset=0;
    targetOffset=0;
    fade_step=0;
    fadeSpeed=0;

    taskToTransform=-1;
    taskToTransform_t=0;
}

LVL_Block::~LVL_Block()
{
    if(physBody && worldPtr)
    {
      worldPtr->DestroyBody(physBody);
      physBody->SetUserData(NULL);
      physBody = NULL;
    }
}

//float LVL_Block::posX()
//{
//    return data.x;
//}

//float LVL_Block::posY()
//{
//    return data.y;
//}

void LVL_Block::init()
{
    if(!worldPtr) return;
    setSize(data.w, data.h);

    slippery = data.slippery;

    sizable = setup->sizable;

    isHidden = data.invisible;

    collide = COLLISION_ANY;

    slippery_surface = data.slippery;

    if((setup->sizable) || (setup->collision==2))
    {
        collide = COLLISION_TOP;
    }
    else
    if(setup->collision==0)
    {
        collide = COLLISION_NONE;
    }

    if(physBody==NULL)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set( PhysUtil::pix2met( data.x+posX_coefficient ),
            PhysUtil::pix2met(data.y + posY_coefficient ) );
        bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);
        physBody = worldPtr->CreateBody(&bodyDef);
    }

    if(f_block!=NULL)
    {
        physBody->DestroyFixture(f_block);
        f_block=NULL;
    }

    if(f_edge!=NULL)
    {
        physBody->DestroyFixture(f_edge);
        f_edge=NULL;
    }

    b2PolygonShape shape;

    switch(setup->phys_shape)
    {
    //triangles
    case 1: //Top-Right triangle
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(posY_coefficient));
          //left-bottom
          vertices[2].Set(PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case -1: //Top-Left triangle
        {
          b2Vec2 vertices[3];
          //Left-bottom
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[2].Set(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case 2: //Bottom-Right triangle
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //left-bottom
          vertices[2].Set(PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case -2: //Bottom-Left triangle
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[2].Set(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          isRectangle = false;
          break;
        }
    case 3:// map shape from texture
        //Is not implemented, create the box:

    //rectangle box
    default: //Rectangle box shape
        {
            shape.SetAsBox(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient) );
            b2EdgeShape edgeShape;
            edgeShape.Set( b2Vec2(PhysUtil::pix2met(-posX_coefficient-0.1), PhysUtil::pix2met(-posY_coefficient-0.1)),
                           b2Vec2(PhysUtil::pix2met(posX_coefficient+0.1), PhysUtil::pix2met(-posY_coefficient-0.1)) );


            if(collide!=COLLISION_TOP)
            {
                edgeShape.m_vertex0.Set( PhysUtil::pix2met(-posX_coefficient-0.1), PhysUtil::pix2met(posY_coefficient+0.1) );
                edgeShape.m_vertex3.Set( PhysUtil::pix2met(posX_coefficient+0.1), PhysUtil::pix2met(posY_coefficient+0.1) );
            }
            else
            {
                edgeShape.m_vertex0.Set( PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(-posY_coefficient) );
                edgeShape.m_vertex3.Set( PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(-posY_coefficient) );
            }
            edgeShape.m_hasVertex0 = true;
            edgeShape.m_hasVertex3 = true;

            f_edge = physBody->CreateFixture(&edgeShape, 1.0f);
            f_edge->SetFriction(slippery_surface? 0.04f : 0.25f );
            if(setup->algorithm==3)
            {
                f_edge->SetSensor(true);
            }
            if(collide==COLLISION_NONE)// || collide==COLLISION_TOP)
                f_edge->SetSensor(true);
        }
    }

    f_block = physBody->CreateFixture(&shape, 1.0f);

    if(setup->algorithm==3)
        {
            f_block->SetSensor(true);
            ConfigManager::Animator_Blocks[animator_ID].setFrames(1, -1);
        }

    if(collide==COLLISION_NONE)// || collide==COLLISION_TOP)
        f_block->SetSensor(true);
    f_block->SetFriction(slippery_surface? 0.04f : 0.25f );
}


void LVL_Block::transformTo(long id, int type)
{
    if(id<=0) return;

    if(type==2)//Other block
    {
        transformTask_block t;
        t.block = this;
        t.id=id;
        t.type=type;

        LvlSceneP::s->block_transfors.push_back(t);
    }
    if(type==1)//Other NPC
    {
        // :-P
    }
}

void LVL_Block::transformTo_x(long id)
{
    data.id = id;

    //LevelScene::zCounter;
    if(setup->sizable)
    {
        z_index = LevelScene::Z_blockSizable +
                ((double)data.y/(double)100000000000) + 1 -
                ((double)data.w * (double)0.0000000000000001);
    }
    else
    {

        if(setup->view==1)
            z_index = LevelScene::Z_BlockFore;
        else
            z_index = LevelScene::Z_Block;
        LevelScene::zCounter += 0.0000000000001;
        z_index += LevelScene::zCounter;
    }
    long tID = ConfigManager::getBlockTexture(data.id);
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        animated = ConfigManager::lvl_block_indexes[data.id].animated;
        animator_ID = ConfigManager::lvl_block_indexes[data.id].animator_ID;
    }

    if(!setup->sizable)
    {
        data.w = texture.w;
        data.h = (texture.h/setup->frames);
    }
}


void LVL_Block::render(float camX, float camY)
{
    //Don't draw hidden block before it will be hitten
    if(isHidden) return;
    if(destroyed) return;

    QRectF blockG = QRectF(posX()-camX+offset_x,
                           posY()-camY+offset_y,
                           width,
                           height);


    AniPos x(0,1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_Blocks[animator_ID].image();

    glEnable(GL_TEXTURE_2D);
    glColor4f( 1.f, 1.f, 1.f, 1.f);

    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(sizable)
    {
        int w = width;
        int h = height;

        int x,y, x2, y2, i, j;
        int hc, wc;

        x = qRound(qreal(texture.w)/3);  // Width of one piece
        y = qRound(qreal(texture.h)/3); // Height of one piece

        //Double size
        x2 = x<<1;
        y2 = y<<1;

        int pWidth = texture.w-x2;//Width of center piece
        int pHeight = texture.h-y2;//Height of center piece

        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width

        int dX=0; //Draw Offset. This need for crop junk on small sizes
        int dY=0;

        if(w < x2) dX = (x2-w)>>1; else dX=0;
        if(h < y2) dY = (y2-h)>>1; else dY=0;

        int totalW=((w-x2)/pWidth);
        int totalH=((h-y2)/pHeight);

        //L Draw left border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<totalH; i++ )
            {
                drawPiece(blockG, QRectF(0, x+hc, x-dX, pHeight), QRectF(0, y, x-dX, pHeight));
                hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(0, x+hc, x-dX, fLnt), QRectF(0, y, x-dX, fLnt));
        }

        //T Draw top border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<totalW; i++ )
            {
                drawPiece(blockG, QRectF(x+hc, 0, pWidth, y-dY), QRectF(x, 0, pWidth, y-dY));
                    hc+=pWidth;
            }
                fLnt = (w-(x2))%pWidth;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(x+hc, 0, fLnt, y-dY), QRectF(x, 0, fLnt, y-dY));
        }

        //B Draw bottom border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<totalW; i++ )
            {
                drawPiece(blockG, QRectF(x+hc, h-y+dY, pWidth, y-dY), QRectF(x, texture.h-y+dY, pWidth, y-dY));
                    hc+=pWidth;
            }
                fLnt = (w-(x2))%pWidth;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(x+hc, h-y+dY, fLnt, y-dY), QRectF(x, texture.h-y+dY, fLnt, y-dY));
        }

        //R Draw right border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<totalH; i++ )
            {
                drawPiece(blockG, QRectF(w-x+dX, y+hc, x-dX, pHeight), QRectF(texture.w-x+dX, y, x-dX, pHeight));
                    hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0)
                    drawPiece(blockG, QRectF(w-x+dX, y+hc, x-dX, fLnt), QRectF(texture.w-x+dX, y, x-dX, fLnt));
        }


        //C Draw center
        if( (w > (x2)) && (h > (y2)))
        {
            hc=0;
            wc=0;
            for(i=0; i< totalH; i++ )
            {
                hc=0;
                for(j=0; j< totalW; j++ )
                {
                    drawPiece(blockG, QRectF(x+hc, y+wc, pWidth, pHeight), QRectF(x, y, pWidth, pHeight));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(blockG, QRectF(x+hc, y+wc, fLnt, pHeight), QRectF(x, y, fLnt, pHeight));
                wc+=pHeight;
            }

            fWdt = (h-y2)%pHeight;
            if(fWdt !=0)
            {
                hc=0;
                for(j=0; j<totalW; j++ )
                {
                    drawPiece(blockG, QRectF(x+hc, y+wc, pWidth, fWdt), QRectF(x, y, pWidth, fWdt));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(blockG, QRectF(x+hc, y+wc, fLnt, fWdt), QRectF(x, y, fLnt, fWdt));
            }

        }

        //Draw corners
         //1 Left-top
        drawPiece(blockG, QRectF(0,0,x-dX,y-dY), QRectF(0,0,x-dX, y-dY));
         //2 Right-top
        drawPiece(blockG, QRectF(w-x+dX, 0, x-dX, y-dY), QRectF(texture.w-x+dX, 0, x-dX, y-dY));
         //3 Right-bottom
        drawPiece(blockG, QRectF(w-x+dX, h-y+dY, x-dX, y-dY), QRectF(texture.w-x+dX, texture.h-y+dY, x-dX, y-dY));
         //4 Left-bottom
        drawPiece(blockG, QRectF(0, h-y+dY, x-dX, y-dY), QRectF(0, texture.h-y+dY, x-dX, y-dY));

    }
    else
    {
        glBegin( GL_QUADS );
            glTexCoord2f( 0, x.first );
            glVertex2f( blockG.left(), blockG.top());

            glTexCoord2f( 1, x.first );
            glVertex2f(  blockG.right(), blockG.top());

            glTexCoord2f( 1, x.second );
            glVertex2f(  blockG.right(),  blockG.bottom());

            glTexCoord2f( 0, x.second );
            glVertex2f( blockG.left(),  blockG.bottom());
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void LVL_Block::drawPiece(QRectF target, QRectF block, QRectF texture)
{
    QRectF tx;
    tx.setLeft(texture.left()/this->texture.w);
    tx.setRight(texture.right()/this->texture.w);
    tx.setTop(texture.top()/this->texture.h);
    tx.setBottom(texture.bottom()/this->texture.h);

    QRectF blockG;
    blockG.setX(target.x()+block.x());
    blockG.setY(target.y()+block.y());
    blockG.setRight(target.x()+block.x()+block.width());
    blockG.setBottom(target.y()+block.y()+block.height());

    glBegin( GL_QUADS );
        glTexCoord2f( tx.left(), tx.top() );
        glVertex2f( blockG.left(), blockG.top());

        glTexCoord2f( tx.right(), tx.top() );
        glVertex2f(  blockG.right(), blockG.top());

        glTexCoord2f( tx.right(), tx.bottom() );
        glVertex2f(  blockG.right(),  blockG.bottom());

        glTexCoord2f( tx.left(), tx.bottom() );
        glVertex2f( blockG.left(),  blockG.bottom());
    glEnd();

}




void LVL_Block::hit(LVL_Block::directions _dir)
{
    hitDirection = _dir;
    isHidden=false;
    data.invisible=false;
    bool doFade=false;

    if((setup->destroyable)&&(data.npc_id==0))
    {
        destroyed=true;
        return;
    }

    if(data.npc_id<0)
    {
        //Coin!
        data.npc_id++;
        doFade=true;
        if((!setup->bounce)&&(!setup->switch_Button))
        {
            if(data.npc_id==0)
                transformTo(setup->transfororm_on_hit_into, 2);
        }
    }
    else
    if(data.npc_id>0)
    {
        //Coin!
        data.npc_id=0;
        doFade=true;
        if((!setup->bounce)&&(!setup->switch_Button))
        {
            transformTo(setup->transfororm_on_hit_into, 2);
        }
    }

    if(setup->switch_Button)
    {
        LvlSceneP::s->toggleSwitch(setup->switch_ID);
    }

    if(setup->hitable)
    {
        transformTo(setup->spawn_obj_id, setup->spawn_obj);
        doFade=true;
    }

    if(doFade)
    {
        fadeOffset=0.f;
        setFade(20, 1.0f, 0.25f);
    }
}


/**************************Fader*******************************/
void LVL_Block::setFade(int speed, float target, float step)
{
    if(fader_timer_id)
        SDL_RemoveTimer(fader_timer_id);

    fade_step = fabs(step);
    targetOffset = target;
    fadeSpeed = speed;

    fader_timer_id = SDL_AddTimer(speed, &LVL_Block::nextOpacity, this);
    if(!fader_timer_id) fadeOffset = targetOffset;
}

unsigned int LVL_Block::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    LVL_Block *self = reinterpret_cast<LVL_Block *>(p);
    self->fadeStep();
    return 0;
}

void LVL_Block::fadeStep()
{
    if(fadeOffset < targetOffset)
        fadeOffset+=fade_step;
    else
        fadeOffset-=fade_step;

    if(fadeOffset>=1.0f || fadeOffset<=0.0f)
    {
        SDL_RemoveTimer(fader_timer_id);
        if(fadeOffset>=1.0f)
            setFade(fadeSpeed, 0.0f, fade_step);
    }
    else
        {
            fader_timer_id = SDL_AddTimer(fadeSpeed, &LVL_Block::nextOpacity, this);
            if(!fader_timer_id) fadeOffset = targetOffset;
        }

    if(fadeOffset>1.0f) fadeOffset = 1.0f;
    else
    if(fadeOffset<0.0f) fadeOffset = 0.0f;

    switch(hitDirection)
    {
        case up:      offset_y = -16*fadeOffset; break;
        case down:    offset_y = 16*fadeOffset; break;
        case left:    offset_x = -16*fadeOffset; break;
        case right:   offset_x = 16*fadeOffset; break;
        default: break;
    }

}
/**************************Fader**end**************************/
