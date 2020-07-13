/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_RHI_RENDERCOMMAND_H
#define QT3DRENDER_RENDER_RHI_RENDERCOMMAND_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qglobal.h>
#include <shaderparameterpack_p.h>
#include <rhihandle_types_p.h>
#include <Qt3DCore/private/vector_helper_p.h>
#include <Qt3DRender/private/renderviewjobutils_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QtGui/private/qrhi_p.h>
#include <Qt3DCore/qattribute.h>

QT_BEGIN_NAMESPACE
class QRhiGraphicsPipeline;
class QRhiShaderResourceBindings;

namespace Qt3DRender {

namespace Render {

class RenderStateSet;
using RenderStateSetPtr = QSharedPointer<RenderStateSet>;

enum RebuildFlag {
    FullCommandRebuild = 1 << 0,
    LayerCacheRebuild = 1 << 1,
    MaterialCacheRebuild = 1 << 2,
    LightCacheRebuild = 1 << 3
};
Q_DECLARE_FLAGS(RebuildFlagSet, RebuildFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(RebuildFlagSet)

namespace Rhi {

class RHIShader;
class RHIGraphicsPipeline;

struct CommandUBO
{
    float modelMatrix[16];
    float inverseModelMatrix[16];
    float modelViewMatrix[16];
    float modelNormalMatrix[12];
    float inverseModelViewMatrix[16];
    float mvp[16];
    float inverseModelViewProjectionMatrix[16];
    float modelViewNormalMatrix[12];
};
static_assert(sizeof(CommandUBO) == 6 * (16 * sizeof(float)) + 2 * (12 * sizeof(float)),
              "UBO doesn't match std140");

struct Q_AUTOTEST_EXPORT AttributeInfo
{
    int nameId = -1;
    QRhiVertexInputBinding::Classification classification = QRhiVertexInputBinding::PerVertex;
    size_t stride = 0;
    size_t offset = 0;
    size_t divisor = 0;
};

Q_AUTOTEST_EXPORT bool operator==(const AttributeInfo &a, const AttributeInfo &b);
Q_AUTOTEST_EXPORT bool operator!=(const AttributeInfo &a, const AttributeInfo &b);

class Q_AUTOTEST_EXPORT RenderCommand
{
public:
    RenderCommand();

    bool isValid() const noexcept;

    HMaterial m_material; // Purely used to ease sorting (minimize stage changes, binding changes
                          // ....)
    RHIShader *m_rhiShader; // GL Shader to be used at render time
    Qt3DCore::QNodeId m_shaderId; // Shader for given pass and mesh
    ShaderParameterPack m_parameterPack; // Might need to be reworked so as to be able to destroy
                                         // the Texture while submission is happening.
    RenderStateSetPtr m_stateSet;

    HGeometry m_geometry;
    HGeometryRenderer m_geometryRenderer;

    HBuffer m_indirectDrawBuffer; // Reference to indirect draw buffer (valid only m_drawIndirect ==
                                  // true)
    HComputeCommand m_computeCommand;

    // A QAttribute pack might be interesting
    // This is a temporary fix in the meantime, to remove the hacked methods in Technique
    std::vector<int> m_activeAttributes;

    float m_depth;
    int m_changeCost;

    enum CommandType { Draw, Compute };

    CommandType m_type;
    int m_workGroups[3];

    // Values filled for draw calls by Renderer (in prepare Submission)
    GLsizei m_primitiveCount;
    QGeometryRenderer::PrimitiveType m_primitiveType;
    int m_restartIndexValue;
    int m_firstInstance;
    int m_firstVertex;
    int m_verticesPerPatch;
    int m_instanceCount;
    int m_indexOffset;
    uint m_indexAttributeByteOffset;
    Qt3DCore::QAttribute::VertexBaseType m_indexAttributeDataType;
    uint m_indirectAttributeByteOffset;
    bool m_drawIndexed;
    bool m_drawIndirect;
    bool m_primitiveRestartEnabled;
    bool m_isValid;

    std::vector<AttributeInfo> m_attributeInfo;
    QVarLengthArray<QRhiCommandBuffer::VertexInput, 8> vertex_input;

    const Attribute *indexAttribute {};
    QRhiBuffer *indexBuffer {};

    CommandUBO m_commandUBO;
    RHIGraphicsPipeline *pipeline {};
};

Q_AUTOTEST_EXPORT bool operator==(const RenderCommand &a, const RenderCommand &b) noexcept;

inline bool operator!=(const RenderCommand &lhs, const RenderCommand &rhs) noexcept
{
    return !operator==(lhs, rhs);
}

} // namespace Rhi

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RHI_RENDERCOMMAND_H
