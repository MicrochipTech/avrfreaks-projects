/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file graphics_sdl.h
    \brief SDL Graphics driver implementation
*/

#include "kerneltypes.h"
#include "driver.h"
#include "graphics.h"
#include "graphics_sdl.h"
#include "font.h"

#include <stdint.h>
#include <SDL/SDL.h>
//---------------------------------------------------------------------------
void GraphicsSDL::Init(void)
{
    m_usResX = 128;
    m_usResY = 128;
    m_ucBPP = 32;
    m_usLeft = 0;
    m_usRight = 127;
    m_usTop = 0;
    m_usBottom = 127;
}

//---------------------------------------------------------------------------
K_UCHAR GraphicsSDL::Open()
{
    SDL_Init( SDL_INIT_EVERYTHING );
    m_pstScreen = SDL_SetVideoMode( 128, 128, 32, SDL_SWSURFACE );
    return 0;
}

//---------------------------------------------------------------------------
K_UCHAR GraphicsSDL::Close()
{
    if (m_pstScreen)
    {
        SDL_FreeSurface(m_pstScreen);
    }
    SDL_Quit();
    return 0;
}

//---------------------------------------------------------------------------
void GraphicsSDL::DrawPixel(DrawPoint_t *pstPoint_)
{
    uint32_t *pixels = (uint32_t*)m_pstScreen->pixels;
    if (((pstPoint_->usX < m_usResX) && (pstPoint_->usY < m_usResY)) &&
        ((pstPoint_->usX >= m_usLeft ) && (pstPoint_->usY >= m_usTop)) &&
        ((pstPoint_->usX <= m_usRight) && (pstPoint_->usY <= m_usBottom)))
    {
        pixels[ ((uint32_t)(pstPoint_->usY) * m_pstScreen->w) + (uint32_t)(pstPoint_->usX) ] = (uint32_t)pstPoint_->uColor;
    }
}

//---------------------------------------------------------------------------
void GraphicsSDL::ReadPixel(DrawPoint_t *pstPoint_)
{
    uint32_t *pixels = (uint32_t*)m_pstScreen->pixels;
    pstPoint_->uColor = (COLOR)pixels[ ((uint32_t)(pstPoint_->usY) * m_pstScreen->w) + (uint32_t)(pstPoint_->usX) ];
}

//---------------------------------------------------------------------------
void GraphicsSDL::Flip()
{
    if (m_pstScreen)
    {
        SDL_Flip(m_pstScreen);
    }
}

