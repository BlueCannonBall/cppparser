/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef GrDistanceFieldAdjustTable_DEFINED
#  define GrDistanceFieldAdjustTable_DEFINED
#  include "include/core/SkRefCnt.h"
#  include "include/core/SkScalar.h"
struct GrDistanceFieldAdjustTable : public SkNVRefCnt<GrDistanceFieldAdjustTable>
{
  GrDistanceFieldAdjustTable()
  {
    this->buildDistanceAdjustTables();
  }
  ~GrDistanceFieldAdjustTable()
  {
    delete[] fTable;
    delete[] fGammaCorrectTable;
  }
  const SkScalar& getAdjustment(int i, bool useGammaCorrectTable) const
  {
    return useGammaCorrectTable ? fGammaCorrectTable[i] : fTable[i];
  }
private:
  void buildDistanceAdjustTables();
  SkScalar* fTable;
  SkScalar* fGammaCorrectTable;
};
#endif
