// Filename: OptionsScreen.c
// Contains code for the functions that are specific to the options screen.
//
// Project Codename: GameB
// TODO: Come up with a better name later.
// 2020 Joseph Ryan Ries <ryanries09@gmail.com>
// My YouTube series where we program an entire video game from scratch in C.
// Watch it on YouTube:    https://www.youtube.com/watch?v=3zFFrBSdBvA
// Follow along on GitHub: https://github.com/ryanries/GameB
// Find me on Twitter @JosephRyanRies 
// # License
// ----------
// The source code in this project is licensed under the MIT license.
// The media assets such as artwork, custom fonts, music and sound effects are licensed under a separate license.
// A copy of that license can be found in the 'Assets' directory.
// stb_vorbis by Sean Barrett is public domain and a copy of its license can be found in the stb_vorbis.c file.

#include "Main.h"

#include "OptionsScreen.h"

MENUITEM gMI_OptionsScreen_SFXVolume = { "SFX Volume:", (GAME_RES_WIDTH / 2) - ((11 * 6) / 2) - 16, 100, TRUE, MenuItem_OptionsScreen_SFXVolume };

MENUITEM gMI_OptionsScreen_MusicVolume = { "Music Volume:", (GAME_RES_WIDTH / 2) - ((13 * 6) / 2) - 16, 115, TRUE, MenuItem_OptionsScreen_MusicVolume };

MENUITEM gMI_OptionsScreen_ScreenSize = { "Screen Size:", (GAME_RES_WIDTH / 2) - ((12 * 6) / 2) - 16, 130, TRUE, MenuItem_OptionsScreen_ScreenSize };

MENUITEM gMI_OptionsScreen_Back = { "Back", (GAME_RES_WIDTH / 2) - ((4 * 6) / 2) - 16, 145, TRUE, MenuItem_OptionsScreen_Back };

MENUITEM* gMI_OptionsScreenItems[] = { &gMI_OptionsScreen_SFXVolume, &gMI_OptionsScreen_MusicVolume, &gMI_OptionsScreen_ScreenSize, &gMI_OptionsScreen_Back };

MENU gMenu_OptionsScreen = { "Options", 0, _countof(gMI_OptionsScreenItems), gMI_OptionsScreenItems };

void DrawOptionsScreen(void)
{
    PIXEL32 Grey = { 0x6F, 0x6F, 0x6F, 0x6F };

    static uint64_t LocalFrameCounter;

    static uint64_t LastFrameSeen;

    static PIXEL32 TextColor;

    char ScreenSizeString[64] = { 0 };

    // If global TotalFramesRendered is greater than LastFrameSeen,
    // that means we have either just entered this gamestate for the first time,
    // or we have left this gamestate previously and have just come back to it.
    // For example we may have gone from the title screen, to the options screen,
    // and then back to the title screen again. In that case, we want to reset all
    // of the "local state," i.e., things that are local to this game state. Such
    // as text animation, selected menu item, etc.
    if (gPerformanceData.TotalFramesRendered > (LastFrameSeen + 1))
    {
        LocalFrameCounter = 0;

        gMenu_OptionsScreen.SelectedItem = 0;

        gInputEnabled = FALSE;
    }

    memset(gBackBuffer.Memory, 0, GAME_DRAWING_AREA_MEMORY_SIZE);

    ApplyFadeIn(LocalFrameCounter, COLOR_TEXT, &TextColor, NULL);

    for (uint8_t MenuItem = 0; MenuItem < gMenu_OptionsScreen.ItemCount; MenuItem++)
    {
        if (gMenu_OptionsScreen.Items[MenuItem]->Enabled == TRUE)
        {
            BlitStringToBuffer(gMenu_OptionsScreen.Items[MenuItem]->Name,
                &g6x7Font,
                &TextColor,
                gMenu_OptionsScreen.Items[MenuItem]->x,
                gMenu_OptionsScreen.Items[MenuItem]->y);
        }
    }

    for (uint8_t Volume = 0; Volume < 10; Volume++)
    {
        if (Volume >= (uint8_t)(gSFXVolume * 10))
        {
            if (TextColor.Colors.Red == 255)
            {
                BlitStringToBuffer("\xf2", &g6x7Font, &Grey, 224 + (Volume * 6), gMI_OptionsScreen_SFXVolume.y);
            }
        }
        else
        {
            BlitStringToBuffer("\xf2", &g6x7Font, &TextColor, 224 + (Volume * 6), gMI_OptionsScreen_SFXVolume.y);
        }
    }

    for (uint8_t Volume = 0; Volume < 10; Volume++)
    {
        if (Volume >= (uint8_t)(gMusicVolume * 10))
        {
            if (TextColor.Colors.Red == 255)
            {
                BlitStringToBuffer("\xf2", &g6x7Font, &Grey, 224 + (Volume * 6), gMI_OptionsScreen_MusicVolume.y);
            }
        }
        else
        {
            BlitStringToBuffer("\xf2", &g6x7Font, &TextColor, 224 + (Volume * 6), gMI_OptionsScreen_MusicVolume.y);
        }
    }

    snprintf(ScreenSizeString,
        sizeof(ScreenSizeString),
        "%dx%d",
        GAME_RES_WIDTH * gPerformanceData.CurrentScaleFactor,
        GAME_RES_HEIGHT * gPerformanceData.CurrentScaleFactor);

    BlitStringToBuffer(ScreenSizeString, &g6x7Font, &TextColor, 224, gMI_OptionsScreen_ScreenSize.y);

    BlitStringToBuffer("�",
        &g6x7Font,
        &TextColor,
        gMenu_OptionsScreen.Items[gMenu_OptionsScreen.SelectedItem]->x - 6,
        gMenu_OptionsScreen.Items[gMenu_OptionsScreen.SelectedItem]->y);

    LocalFrameCounter++;

    LastFrameSeen = gPerformanceData.TotalFramesRendered;
}

void PPI_OptionsScreen(void)
{
    if (gGameInput.DownKeyIsDown && !gGameInput.DownKeyWasDown)
    {
        if (gMenu_OptionsScreen.SelectedItem < gMenu_OptionsScreen.ItemCount - 1)
        {
            gMenu_OptionsScreen.SelectedItem++;

            PlayGameSound(&gSoundMenuNavigate);
        }
    }

    if (gGameInput.UpKeyIsDown && !gGameInput.UpKeyWasDown)
    {
        if (gMenu_OptionsScreen.SelectedItem > 0)
        {
            gMenu_OptionsScreen.SelectedItem--;

            PlayGameSound(&gSoundMenuNavigate);
        }
    }

    
    if ((gGameInput.ChooseKeyIsDown && !gGameInput.ChooseKeyWasDown))
    {
        gMenu_OptionsScreen.Items[gMenu_OptionsScreen.SelectedItem]->Action();

        PlayGameSound(&gSoundMenuChoose);
    }
    else if ((gGameInput.LeftKeyIsDown && !gGameInput.LeftKeyWasDown) ||        
             (gGameInput.RightKeyIsDown && !gGameInput.RightKeyWasDown))    
    {
        // Allow the left and right buttons to work in the options menu,
        // but not on the back button. Must still use the choose key to go back.
        if (gMenu_OptionsScreen.SelectedItem != gMenu_OptionsScreen.ItemCount - 1)
        {
            gMenu_OptionsScreen.Items[gMenu_OptionsScreen.SelectedItem]->Action();

            PlayGameSound(&gSoundMenuChoose);
        }
    }

    if (gGameInput.EscapeKeyIsDown && !gGameInput.EscapeKeyWasDown)
    {
        MenuItem_OptionsScreen_Back();

        PlayGameSound(&gSoundMenuChoose);
    }
}

void MenuItem_OptionsScreen_Back(void)
{
    gCurrentGameState = gPreviousGameState;

    gPreviousGameState = GAMESTATE_OPTIONSSCREEN;

    if (SaveRegistryParameters() != ERROR_SUCCESS)
    {
        LogMessageA(LL_ERROR, "[%s] SaveRegistryParameters failed!", __FUNCTION__);
    }
}

void MenuItem_OptionsScreen_ScreenSize(void)
{
    if (gGameInput.LeftKeyIsDown && !gGameInput.LeftKeyWasDown)
    {
        gPerformanceData.CurrentScaleFactor--;        
    }
    else
    {
        gPerformanceData.CurrentScaleFactor++;
    }

    if (gPerformanceData.CurrentScaleFactor < 1)
    {
        gPerformanceData.CurrentScaleFactor = gPerformanceData.MaxScaleFactor;
    }

    if (gPerformanceData.CurrentScaleFactor > gPerformanceData.MaxScaleFactor)
    {
        gPerformanceData.CurrentScaleFactor = 1;
    }

    InvalidateRect(gGameWindow, NULL, TRUE);
}

void MenuItem_OptionsScreen_MusicVolume(void)
{
    if (gGameInput.LeftKeyIsDown && !gGameInput.LeftKeyWasDown)
    {
        gMusicVolume -= 0.1f;
    }
    else
    {
        gMusicVolume += 0.1f;
    }

    if (gMusicVolume < 0.0f)
    {
        gMusicVolume = 1.0f;
    }  

    if ((uint8_t)(gMusicVolume * 10) > 10)
    {
        gMusicVolume = 0.0f;
    }

    gXAudioMusicSourceVoice->lpVtbl->SetVolume(gXAudioMusicSourceVoice, gMusicVolume, XAUDIO2_COMMIT_NOW);
}

void MenuItem_OptionsScreen_SFXVolume(void)
{
    if (gGameInput.LeftKeyIsDown && !gGameInput.LeftKeyWasDown)
    {
        gSFXVolume -= 0.1f;
    }
    else
    {
        gSFXVolume += 0.1f;
    }

    if (gSFXVolume < 0.0f)
    {
        gSFXVolume = 1.0f;
    }

    if ((uint8_t)(gSFXVolume * 10) > 10)
    {
        gSFXVolume = 0.0f;
    }

    for (uint8_t Counter = 0; Counter < NUMBER_OF_SFX_SOURCE_VOICES; Counter++)
    {
        gXAudioSFXSourceVoice[Counter]->lpVtbl->SetVolume(gXAudioSFXSourceVoice[Counter], gSFXVolume, XAUDIO2_COMMIT_NOW);
    }
}