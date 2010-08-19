/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of Michael A. Jackson nor the names of its contributors may
* be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef CROSSCORRELATIONPLUGIN_H_
#define CROSSCORRELATIONPLUGIN_H_

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include "TO79/plugins/QImageProcessingInterface.h"

class CrossCorrelationInputUI;
/**
 * @class CrossCorrelationPlugin CrossCorrelationPlugin.h CrossCorrelation/CrossCorrelationPlugin.h
 * @brief This plugin implements a Cross Correlation algorithm to register or align
 * a pair of images. The output image will be a possibly shifted (in translation only) version
 * of the 'moving' image. See the parent class QImageProcessingInterface for documentation
 * on each of the methods.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jul 27, 2010
 * @version 1.0
 */
class CrossCorrelationPlugin : public QObject, public QImageProcessingInterface
{
    Q_OBJECT;
    Q_INTERFACES(QImageProcessingInterface )

  public:
    CrossCorrelationPlugin();
    virtual ~CrossCorrelationPlugin();

    /* ---- Implementation of pure virtual functions from QImageProcessingInterface class -- */
    QString getPluginName();

    virtual QWidget* getInputWidget(QWidget* parent);

    virtual int startProcessing(QObject* caller);

    virtual QString getInputImage();
    virtual QString getProcessedImage();

    virtual void writeSettings(QSettings* prefs);
    virtual void readSettings(QSettings* prefs);
    /* ---- Other class specific methods --- */


  protected:
    CrossCorrelationInputUI* m_InputWidget;

  private:
    CrossCorrelationPlugin(const CrossCorrelationPlugin&); // Copy Constructor Not Implemented
    void operator=(const CrossCorrelationPlugin&); // Operator '=' Not Implemented
};

#endif /* CROSSCORRELATIONPLUGIN_H_ */
