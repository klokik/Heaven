#ifndef BEZIERCURVE_H_
#define BEZIERCURVE_H_

#include <vector>
#include <string>
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
		// look wikipedia's article for description
		T getPoint(size_t _segment_num,float _t)
		{
			if(_t<0)
				throw std::string("invalid parameter: cant be negative");
			size_t segment_num = _segment_num + floor(_t);
			float t = _t - floor(_t);
			//number of segments is equal to num. of correction points (one point per segment)
			if(correction_points.size()>=segment_num && base_points.size()>=2)
			{
				// pivot points
				T P[] = {
					base_points[segment_num],
					correction_points[segment_num],
					base_points[segment_num+1]
				};

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
			else
				throw std::out_of_range("not enought points to calculate provided segment_num");
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

		float unitLength(void)
		{
			// TODO
			return correction_points.size();
		}
	};
}

#endif /* BEZIERCURVE_H_ */