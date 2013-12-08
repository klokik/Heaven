#ifndef BEZIERCURVE_H_
#define BEZIERCURVE_H_

#include <vector>
#include <string>
#include <tuple>
#include <stdexcept>
#include <cmath>
#include <cstring>

namespace heaven
{
	template<class T>
	class BezierQuadCurve
	{
	protected:
		std::vector<T> base_points;
		std::vector<T> correction_points;

	public:
		std::tuple<T,T,T,float> getSegment(size_t _segment_num,float _t = 0)
		{
			if(_t<0)
				throw std::string("invalid parameter: can't be negative");
			size_t segment_num = _segment_num + floor(_t);
			float t = _t - floor(_t);
			//number of segments is equal to num. of correction points (one point per segment)
			if(correction_points.size()>=segment_num && base_points.size()>=2)
			{
				// pivot points
				return std::make_tuple(
					base_points[segment_num],
					correction_points[segment_num],
					base_points[segment_num+1],
					t
				);
			}
			else
				throw std::out_of_range("not enought points to calculate provided segment_num");
		}

		// look wikipedia's article for description
		T getPoint(size_t _segment_num,float _t)
		{
			// pivot points
			T P[3];
			//truncated
			float t;

			std::tie(P[0],P[1],P[2],t) = getSegment(_segment_num,_t);

			// Bernstain's polynomials
			float b[] = {
				std::pow(1-t,2.0f),
				2*t*(1-t),
				std::pow(t,2.0f)
			};

			T sum;
			memset(&sum,0,sizeof(T));

			size_t cnt = 3;

			for(size_t i=0;i<cnt;++i)
				sum = sum + P[i]*b[i];

			return sum;
		}

		// approximate form
		T getUniformPoint(size_t _segment_num, float _t)
		{
			// pivot points
			T P[3];
			//truncated
			float t;

			std::tie(P[0],P[1],P[2],t) = getSegment(_segment_num,_t);

			//FIXME: incorrect
			float new_t = t+1/Length((P[0]*2.0f-P[1]*4.0f+P[2]*2.0f)*t-P[0]*2.0f+P[1]*2.0f);

			return getPoint(_segment_num+(_t-t),new_t);
		}

		void addStartPoint(T pt)
		{
			if(base_points.empty())
				base_points.push_back(pt);
			else
				throw std::string("curve already contains start point");
		}

		void addSegment(T base_pt,T correction_pt)
		{
			if(base_pt!=base_pt)
				throw "FUCK";

			if(!base_points.empty())
			{
				base_points.push_back(base_pt);
				correction_points.push_back(correction_pt);
			}
			else
				throw std::string("first you need to add start point");
		}

		void clear(void)
		{
			base_points.clear();
			correction_points.clear();
		}

		bool isEmpty(void)
		{
			return correction_points.empty();
		}

		float segLength(void)
		{
			return correction_points.size();
		}

		float lenOfSegment(size_t _segment_num)
		{
			// pivot points
			T P[3];

			std::tie(P[0],P[1],P[2],std::ignore) = getSegment(_segment_num);

			T A = P[0] - P[1]*2.0f + P[2];
			T B = P[1] - P[0];
			float a = dot(A,A);
			float b = dot(A,B);
			float c = dot(B,B);

			// integral from 0 to 1 of |B'(t)| by dt
			float length = ((a*c-b*b)*std::log(std::sqrt(a*(a+2*b+c))+a+b)+std::sqrt(a*(a+2*b+c))*(a+b))/std::pow(a,1.5f);

			return length;
		}

		float curveLength(void)
		{
			float sum = 0;

			for(size_t i=0;i<correction_points.size();++i)
				sum += lenOfSegment(i);

			return sum;
		}
	};
}

#endif /* BEZIERCURVE_H_ */