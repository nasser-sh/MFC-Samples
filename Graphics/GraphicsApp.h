/*   Copyright (c) nasser-sh 2016
*
*   Distributed under BSD-style license. See accompanying LICENSE.txt in project
*   directory.
*/
#pragma once
#include <afxwin.h>
#include "Settings.h"


namespace graphics
{
    class CGraphicsApp : public CWinApp
    {
    public:
        CGraphicsApp() = default;
        virtual ~CGraphicsApp() = default;

        // overrides
        BOOL InitInstance() override;

        // member functions
        CSettings const &Settings() const;

        // callbacks
        void OnFixedFunctionGLWindow();
        void OnModernGLWindow();
        void OnSettingsDialog();
        void OnUpdateSettings(CSettings const &settings);

    private:
        CSettings m_settings;
    };
}

