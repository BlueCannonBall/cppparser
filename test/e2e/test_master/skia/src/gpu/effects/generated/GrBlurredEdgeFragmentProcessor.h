/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrBlurredEdgeFragmentProcessor.fp; do not modify.
 **************************************************************************************************/
#ifndef GrBlurredEdgeFragmentProcessor_DEFINED
#  define GrBlurredEdgeFragmentProcessor_DEFINED
#  include "include/core/SkTypes.h"
#  include "src/gpu/GrCoordTransform.h"
#  include "src/gpu/GrFragmentProcessor.h"
class GrBlurredEdgeFragmentProcessor : public GrFragmentProcessor
{
public:
  enum class Mode { kGaussian = 0, kSmoothStep = 1 };
  static std::unique_ptr<GrFragmentProcessor> Make(Mode mode)
  {
    return std::unique_ptr<GrFragmentProcessor>(new GrBlurredEdgeFragmentProcessor(mode));
  }
  GrBlurredEdgeFragmentProcessor(const GrBlurredEdgeFragmentProcessor& src);
  std::unique_ptr<GrFragmentProcessor> clone() const override;
  const char* name() const override
  {
    return "BlurredEdgeFragmentProcessor";
  }
  Mode mode;
private:
  GrBlurredEdgeFragmentProcessor(Mode mode)
    : INHERITED(kGrBlurredEdgeFragmentProcessor_ClassID, kNone_OptimizationFlags)
    , mode(mode)
  {
  }
  GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
  void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
  bool onIsEqual(const GrFragmentProcessor&) const override;
  GR_DECLARE_FRAGMENT_PROCESSOR_TEST
  typedef GrFragmentProcessor INHERITED;
};
#endif
