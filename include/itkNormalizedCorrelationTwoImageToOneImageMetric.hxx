/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkNormalizedCorrelationTwoImageToOneImageMetric_hxx
#define itkNormalizedCorrelationTwoImageToOneImageMetric_hxx

#include "itkNormalizedCorrelationTwoImageToOneImageMetric.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{

template <class TFixedImage, class TMovingImage>
NormalizedCorrelationTwoImageToOneImageMetric<TFixedImage,TMovingImage>
::NormalizedCorrelationTwoImageToOneImageMetric()
{
  m_SubtractMean = false;
}


template <class TFixedImage, class TMovingImage>
typename NormalizedCorrelationTwoImageToOneImageMetric<TFixedImage,TMovingImage>::MeasureType
NormalizedCorrelationTwoImageToOneImageMetric<TFixedImage,TMovingImage>
::GetValue( const TransformParametersType & parameters ) const
{

  FixedImageConstPointer fixedImage1 = this->m_FixedImage1;

  if( !fixedImage1 )
    {
    itkExceptionMacro( << "Fixed image1 has not been assigned" );
    }

  FixedImageConstPointer fixedImage2 = this->m_FixedImage2;

  if( !fixedImage2 )
    {
    itkExceptionMacro( << "Fixed image2 has not been assigned" );
    }

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;

  typedef  typename NumericTraits< MeasureType >::AccumulateType AccumulateType;


  // Calculate the measure value between fixed image 1 and the moving image

  FixedIteratorType ti1( fixedImage1, this->GetFixedImageRegion1() );

  typename FixedImageType::IndexType index;

  MeasureType measure1;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  AccumulateType sff = NumericTraits< AccumulateType >::Zero;
  AccumulateType smm = NumericTraits< AccumulateType >::Zero;
  AccumulateType sfm = NumericTraits< AccumulateType >::Zero;
  AccumulateType sf  = NumericTraits< AccumulateType >::Zero;
  AccumulateType sm  = NumericTraits< AccumulateType >::Zero;
  typename Superclass::InputPointType inputPoint;

  while(!ti1.IsAtEnd())
    {

    index = ti1.GetIndex();

    fixedImage1->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask1 && !this->m_FixedImageMask1->IsInside( inputPoint ) )
      {
      ++ti1;
      continue;
      }

//    typename Superclass::OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( inputPoint ) )
      {
      ++ti1;
      continue;
      }

    if( this->m_Interpolator1->IsInsideBuffer( inputPoint ) )
      {
      const RealType movingValue  = this->m_Interpolator1->Evaluate( inputPoint );
      const RealType fixedValue   = ti1.Get();
      sff += fixedValue  * fixedValue;
      smm += movingValue * movingValue;
      sfm += fixedValue  * movingValue;
      if ( this->m_SubtractMean )
        {
        sf += fixedValue;
        sm += movingValue;
        }
      this->m_NumberOfPixelsCounted++;
      }

    ++ti1;
    }

  if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 )
    {
    sff -= ( sf * sf / this->m_NumberOfPixelsCounted );
    smm -= ( sm * sm / this->m_NumberOfPixelsCounted );
    sfm -= ( sf * sm / this->m_NumberOfPixelsCounted );
    }

  RealType denom = -1.0 * sqrt( sff * smm );

  if( this->m_NumberOfPixelsCounted > 0 && denom != 0.0)
    {
    measure1 = sfm / denom;
    }
  else
    {
    measure1 = NumericTraits< MeasureType >::Zero;
    }


  // Calculate the measure value between fixed image 2 and the moving image

  FixedIteratorType ti2( fixedImage2, this->GetFixedImageRegion2() );

  MeasureType measure2;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  sff = NumericTraits< AccumulateType >::Zero;
  smm = NumericTraits< AccumulateType >::Zero;
  sfm = NumericTraits< AccumulateType >::Zero;
  sf  = NumericTraits< AccumulateType >::Zero;
  sm  = NumericTraits< AccumulateType >::Zero;

  while(!ti2.IsAtEnd())
    {

    index = ti2.GetIndex();

//    typename Superclass::InputPointType inputPoint;
    fixedImage2->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask2 && !this->m_FixedImageMask2->IsInside( inputPoint ) )
      {
      ++ti2;
      continue;
      }

//    typename Superclass::OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( inputPoint ) )
      {
      ++ti2;
      continue;
      }

    if( this->m_Interpolator2->IsInsideBuffer( inputPoint ) )
      {
      const RealType movingValue  = this->m_Interpolator2->Evaluate( inputPoint );
      const RealType fixedValue   = ti2.Get();
      sff += fixedValue  * fixedValue;
      smm += movingValue * movingValue;
      sfm += fixedValue  * movingValue;
      if ( this->m_SubtractMean )
        {
        sf += fixedValue;
        sm += movingValue;
        }
      this->m_NumberOfPixelsCounted++;
      }

    ++ti2;
    }

  if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 )
    {
    sff -= ( sf * sf / this->m_NumberOfPixelsCounted );
    smm -= ( sm * sm / this->m_NumberOfPixelsCounted );
    sfm -= ( sf * sm / this->m_NumberOfPixelsCounted );
    }

  denom = -1.0 * sqrt( sff * smm );

  if( this->m_NumberOfPixelsCounted > 0 && denom != 0.0)
    {
    measure2 = sfm / denom;
    }
  else
    {
    measure2 = NumericTraits< MeasureType >::Zero;
    }


  return (measure1 + measure2)/2.0;

}


template < class TFixedImage, class TMovingImage>
void
NormalizedCorrelationTwoImageToOneImageMetric<TFixedImage,TMovingImage>
::GetDerivative( const TransformParametersType & parameters,
                 DerivativeType & derivative ) const
{
	// under construction
}


template <class TFixedImage, class TMovingImage>
void
NormalizedCorrelationTwoImageToOneImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivative(const TransformParametersType & parameters,
                        MeasureType & value, DerivativeType  & derivative) const
{
	// under construction
}


template < class TFixedImage, class TMovingImage>
void
NormalizedCorrelationTwoImageToOneImageMetric<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "SubtractMean: " << m_SubtractMean << std::endl;
}

} // end namespace itk


#endif
