/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef GrRenderTargetProxyPriv_DEFINED
#  define GrRenderTargetProxyPriv_DEFINED
#  include "src/gpu/GrRenderTargetProxy.h"
class GrRenderTargetProxyPriv
{
public:
  void setGLRTFBOIDIs0()
  {
        // FBO0 should never be wrapped as a texture render target.
    SkASSERT(!fRenderTargetProxy->requiresManualMSAAResolve());
    SkASSERT(!fRenderTargetProxy->asTextureProxy());
    fRenderTargetProxy->setGLRTFBOIDIs0();
  }
  bool glRTFBOIDIs0() const
  {
    return fRenderTargetProxy->glRTFBOIDIs0();
  }
private:
  explicit GrRenderTargetProxyPriv(GrRenderTargetProxy* renderTargetProxy)
    : fRenderTargetProxy(renderTargetProxy)
  {
  }
  GrRenderTargetProxyPriv(const GrRenderTargetProxyPriv&)
  {
  }
  GrRenderTargetProxyPriv& operator=(const GrRenderTargetProxyPriv&);
    // No taking addresses of this type.
  const GrRenderTargetProxyPriv* operator&() const;
  GrRenderTargetProxyPriv* operator&();
  GrRenderTargetProxy* fRenderTargetProxy;
  friend class GrRenderTargetProxy;
};
inline GrRenderTargetProxyPriv GrRenderTargetProxy::rtPriv()
{
  return GrRenderTargetProxyPriv(this);
}
inline const GrRenderTargetProxyPriv GrRenderTargetProxy::rtPriv() const
{
  return GrRenderTargetProxyPriv(const_cast<GrRenderTargetProxy*>(this));
}
#endif
