#include "CommonPCH.h"
#include "Common/JSON/JSONVector.h"
#include "Common/Math/VectorFloat4.h"

void to_json(nlohmann::json& j, const VectorFloat4& p)
{
   j = nlohmann::json::array({p[0], p[1], p[2], p[3]});
   return;
}

void from_json(const nlohmann::json& j, VectorFloat4& p)
{
   if (j.is_array())
   {
      p[0] = j[0];
      p[1] = j[1];
      p[2] = j[2];
      p[3] = j[3];
   }
   return;
}
