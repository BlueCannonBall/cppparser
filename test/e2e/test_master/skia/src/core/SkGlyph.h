/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef SkGlyph_DEFINED
#  define SkGlyph_DEFINED
#  include "include/core/SkPath.h"
#  include "include/core/SkTypes.h"
#  include "include/private/SkChecksum.h"
#  include "include/private/SkFixed.h"
#  include "include/private/SkTo.h"
#  include "src/core/SkMask.h"
class SkArenaAlloc;
class SkStrike;
class SkScalerContext;
// needs to be != to any valid SkMask::Format
#  define MASK_FORMAT_UNKNOWN(0xFF)
#  define MASK_FORMAT_JUST_ADVANCE	MASK_FORMAT_UNKNOWN
// A combination of SkGlyphID and sub-pixel position information.
struct SkPackedGlyphID
{
  static constexpr uint32_t kImpossibleID = ~0u;
  enum {
        // Lengths
        kGlyphIDLen     = 16u,
        kSubPixelPosLen = 2u,

        // Bit positions
        kGlyphID   = 0u,
        kSubPixelY = kGlyphIDLen,
        kSubPixelX = kGlyphIDLen + kSubPixelPosLen,
        kEndData   = kGlyphIDLen + 2 * kSubPixelPosLen,

        // Masks
        kGlyphIDMask     = (1u << kGlyphIDLen) - 1,
        kSubPixelPosMask = (1u << kSubPixelPosLen) - 1,
        kMaskAll         = (1u << kEndData) - 1,

        // Location of sub pixel info in a fixed pointer number.
        kFixedPointBinaryPointPos = 16u,
        kFixedPointSubPixelPosBits = kFixedPointBinaryPointPos - kSubPixelPosLen,
  };
  explicit SkPackedGlyphID(SkGlyphID glyphID)
    : fID(glyphID)
  {
  }
  SkPackedGlyphID(SkGlyphID glyphID, SkFixed x, SkFixed y)
    : fID(PackIDXY(glyphID, x, y))
  {
    SkASSERT(fID != kImpossibleID);
  }
  SkPackedGlyphID(SkGlyphID code, SkIPoint pt)
    : SkPackedGlyphID(code, pt.fX, pt.fY)
  {
  }
  SkPackedGlyphID()
    : fID(kImpossibleID)
  {
  }
  bool operator==(const SkPackedGlyphID& that) const
  {
    return fID == that.fID;
  }
  bool operator!=(const SkPackedGlyphID& that) const
  {
    return !(*this == that);
  }
  bool operator<(SkPackedGlyphID that) const
  {
    return this->fID < that.fID;
  }
  uint32_t code() const
  {
    return fID & kGlyphIDMask;
  }
  uint32_t value() const
  {
    return fID;
  }
  SkFixed getSubXFixed() const
  {
    return this->subToFixed(kSubPixelX);
  }
  SkFixed getSubYFixed() const
  {
    return this->subToFixed(kSubPixelY);
  }
  uint32_t hash() const
  {
    return SkChecksum::CheapMix(fID);
  }
  SkString dump() const
  {
    SkString str;
    str.appendf("code: %d, x: %d, y:%d", code(), getSubXFixed(), getSubYFixed());
    return str;
  }
private:
  static constexpr uint32_t PackIDXY(SkGlyphID glyphID, SkFixed x, SkFixed y)
  {
    return (FixedToSub(x) << kSubPixelX) | (FixedToSub(y) << kSubPixelY) | glyphID;
  }
  static constexpr uint32_t FixedToSub(SkFixed n)
  {
    return ((uint32_t) n >> kFixedPointSubPixelPosBits) & kSubPixelPosMask;
  }
  constexpr SkFixed subToFixed(uint32_t subPixelPosBit) const
  {
    uint32_t subPixelPosition = (fID >> subPixelPosBit) & kSubPixelPosMask;
    return subPixelPosition << kFixedPointSubPixelPosBits;
  }
  uint32_t fID;
};
struct SkGlyphPrototype;
class SkGlyph
{
public:
  static constexpr SkFixed kSubpixelRound = SK_FixedHalf >> SkPackedGlyphID::kSubPixelPosLen;
  explicit SkGlyph(SkPackedGlyphID id)
    : fID(id)
  {
  }
  explicit SkGlyph(const SkGlyphPrototype& p);
  SkVector advanceVector() const
  {
    return SkVector(fAdvanceX, fAdvanceY);
  }
  SkScalar advanceX() const
  {
    return fAdvanceX;
  }
  SkScalar advanceY() const
  {
    return fAdvanceY;
  }
  SkGlyphID getGlyphID() const
  {
    return fID.code();
  }
  SkPackedGlyphID getPackedID() const
  {
    return fID;
  }
  SkFixed getSubXFixed() const
  {
    return fID.getSubXFixed();
  }
  SkFixed getSubYFixed() const
  {
    return fID.getSubYFixed();
  }
  size_t rowBytes() const;
  size_t rowBytesUsingFormat(SkMask::Format format) const;
    // Call this to set all of the metrics fields to 0 (e.g. if the scaler
    // encounters an error measuring a glyph). Note: this does not alter the
    // fImage, fPath, fID, fMaskFormat fields.
  void zeroMetrics();
  SkMask mask() const;
  SkMask mask(SkPoint position) const;
    // Image
    // If we haven't already tried to associate an image with this glyph
    // (i.e. setImageHasBeenCalled() returns false), then use the
    // SkScalerContext or const void* argument to set the image.
  bool setImage(SkArenaAlloc* alloc, SkScalerContext* scalerContext);
  bool setImage(SkArenaAlloc* alloc, const void* image);
    // Merge the from glyph into this glyph using alloc to allocate image data. Return true if
    // image data was allocated. If the image for this glyph has not been initialized, then copy
    // the width, height, top, left, format, and image into this glyph making a copy of the image
    // using the alloc.
  bool setMetricsAndImage(SkArenaAlloc* alloc, const SkGlyph& from);
    // Returns true if the image has been set.
  bool setImageHasBeenCalled() const
  {
    return fImage != nullptr || this->isEmpty() || this->imageTooLarge();
  }
    // Return a pointer to the path if the image exists, otherwise return nullptr.
  const void* image() const
  {
    SkASSERT(this->setImageHasBeenCalled());
    return fImage;
  }
    // Return the size of the image.
  size_t imageSize() const;
    // Path
    // If we haven't already tried to associate a path to this glyph
    // (i.e. setPathHasBeenCalled() returns false), then use the
    // SkScalerContext or SkPath argument to try to do so.  N.B. this
    // may still result in no path being associated with this glyph,
    // e.g. if you pass a null SkPath or the typeface is bitmap-only.
    //
    // This setPath() call is sticky... once you call it, the glyph
    // stays in its state permanently, ignoring any future calls.
    //
    // Returns true if this is the first time you called setPath()
    // and there actually is a path; call path() to get it.
  bool setPath(SkArenaAlloc* alloc, SkScalerContext* scalerContext);
  bool setPath(SkArenaAlloc* alloc, const SkPath* path);
    // Returns true if that path has been set.
  bool setPathHasBeenCalled() const
  {
    return fPathData != nullptr;
  }
    // Return a pointer to the path if it exists, otherwise return nullptr. Only works if the
    // path was previously set.
  const SkPath* path() const;
    // Format
  bool isColor() const
  {
    return fMaskFormat == SkMask::kARGB32_Format;
  }
  SkMask::Format maskFormat() const
  {
    return static_cast<SkMask::Format>(fMaskFormat);
  }
  size_t formatAlignment() const;
    // Bounds
  int maxDimension() const
  {
    return std::max(fWidth, fHeight);
  }
  SkIRect iRect() const
  {
    return SkIRect::MakeXYWH(fLeft, fTop, fWidth, fHeight);
  }
  SkRect rect() const
  {
    return SkRect::MakeXYWH(fLeft, fTop, fWidth, fHeight);
  }
  int left() const
  {
    return fLeft;
  }
  int top() const
  {
    return fTop;
  }
  int width() const
  {
    return fWidth;
  }
  int height() const
  {
    return fHeight;
  }
  bool isEmpty() const
  {
        // fHeight == 0 -> fWidth == 0;
    SkASSERT(fHeight != 0 || fWidth == 0);
    return fWidth == 0;
  }
  bool imageTooLarge() const
  {
    return fWidth >= kMaxGlyphWidth;
  }
    // Make sure that the intercept information is on the glyph and return it, or return it if it
    // already exists.
    // * bounds - either end of the gap for the character.
    // * scale, xPos - information about how wide the gap is.
    // * array - accumulated gaps for many characters if not null.
    // * count - the number of gaps.
  void ensureIntercepts(const SkScalar bounds[2], SkScalar scale, SkScalar xPos, SkScalar* array, int* count, SkArenaAlloc* alloc);
private:
    // There are two sides to an SkGlyph, the scaler side (things that create glyph data) have
    // access to all the fields. Scalers are assumed to maintain all the SkGlyph invariants. The
    // consumer side has a tighter interface.
  friend class RandomScalerContext;
  friend class SkScalerContext;
  friend class SkScalerContextProxy;
  friend class SkScalerContext_Empty;
  friend class SkScalerContext_FreeType;
  friend class SkScalerContext_FreeType_Base;
  friend class SkScalerContext_DW;
  friend class SkScalerContext_GDI;
  friend class SkScalerContext_Mac;
  friend class SkStrikeClient;
  friend class SkStrikeServer;
  friend class SkTestScalerContext;
  friend class SkTestSVGScalerContext;
  friend class TestSVGTypeface;
  friend class TestTypeface;
  static constexpr uint16_t kMaxGlyphWidth = 1u << 13u;
    // Support horizontal and vertical skipping strike-through / underlines.
    // The caller walks the linked list looking for a match. For a horizontal underline,
    // the fBounds contains the top and bottom of the underline. The fInterval pair contains the
    // beginning and end of of the intersection of the bounds and the glyph's path.
    // If interval[0] >= interval[1], no intersection was found.
  struct Intercept
  {
    Intercept* fNext;
    SkScalar fBounds[2];
    SkScalar fInterval[2];
  };
  struct PathData
  {
    Intercept* fIntercept{nullptr};
    SkPath fPath;
    bool fHasPath{false};
  };
  size_t allocImage(SkArenaAlloc* alloc);
    // path == nullptr indicates that there is no path.
  void installPath(SkArenaAlloc* alloc, const SkPath* path);
    // The width and height of the glyph mask.
  uint16_t fWidth = 0, fHeight;
    // The offset from the glyphs origin on the baseline to the top left of the glyph mask.
  int16_t fTop = 0, fLeft;
    // fImage must remain null if the glyph is empty or if width > kMaxGlyphWidth.
  void* fImage = nullptr;
    // Path data has tricky state. If the glyph isEmpty, then fPathData should always be nullptr,
    // else if fPathData is not null, then a path has been requested. The fPath field of fPathData
    // may still be null after the request meaning that there is no path for this glyph.
  PathData* fPathData = nullptr;
    // The advance for this glyph.
  float fAdvanceX = 0, fAdvanceY;
    // This is a combination of SkMask::Format and SkGlyph state. The SkGlyph can be in one of two
    // states, just the advances have been calculated, and all the metrics are available. The
    // illegal mask format is used to signal that only the advances are available.
  uint8_t fMaskFormat = MASK_FORMAT_UNKNOWN;
    // Used by the DirectWrite scaler to track state.
  int8_t fForceBW = 0;
  const SkPackedGlyphID fID;
};
struct SkGlyphPrototype
{
  SkPackedGlyphID id;
  float advanceX = 0, advanceY;
    // The width and height of the glyph mask.
  uint16_t width = 0, height;
    // The offset from the glyphs origin on the baseline to the top left of the glyph mask.
  int16_t left = 0, top;
  SkMask::Format maskFormat = SkMask::kBW_Format;
  bool forceBW = false;
};
#endif
