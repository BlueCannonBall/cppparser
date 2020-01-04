/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef GrCoverageSetOpXP_DEFINED
#  define GrCoverageSetOpXP_DEFINED
#  include "include/core/SkRegion.h"
#  include "include/gpu/GrTypes.h"
#  include "src/gpu/GrXferProcessor.h"
#  if  defined(__GNUC__)
#    pragma  GCC diagnostic push
#    pragma  GCC diagnostic ignored "-Wnon-virtual-dtor"
#  endif
#  if  defined(__clang__)
#    pragma  clang diagnostic push
#    pragma  clang diagnostic ignored "-Wnon-virtual-dtor"
#  endif
class GrCoverageSetOpXPFactory : public GrXPFactory
{
public:
  static const GrXPFactory* Get(SkRegion::Op regionOp, bool invertCoverage = false);
private:
  GrCoverageSetOpXPFactory(SkRegion::Op regionOp, bool invertCoverage);
  sk_sp<const GrXferProcessor> makeXferProcessor(const GrProcessorAnalysisColor&, GrProcessorAnalysisCoverage, bool hasMixedSamples, const GrCaps&, GrClampType) const override;
  AnalysisProperties analysisProperties(const GrProcessorAnalysisColor&, const GrProcessorAnalysisCoverage&, const GrCaps&, GrClampType) const override
  {
    return AnalysisProperties::kIgnoresInputColor;
  }
  GR_DECLARE_XP_FACTORY_TEST
  SkRegion::Op fRegionOp;
  bool fInvertCoverage;
  typedef GrXPFactory INHERITED;
};
#  if  defined(__GNUC__)
#    pragma  GCC diagnostic pop
#  endif
#  if  defined(__clang__)
#    pragma  clang diagnostic pop
#  endif
#endif
