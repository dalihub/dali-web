#include <cmath>
#include <cstdarg>
#include <cstdio>

#include <dali/public-api/dali-core.h>
#include "test-application.h"

extern "C"
{
void tet_infoline(const char* text)
{
  puts(text);
}

void tet_printf(const char* format, ...)
{
  va_list arguments;
  va_start(arguments, format);
  vprintf(format, arguments);
  va_end(arguments);
}
}

int main()
{
  // The upstream test controller is a mock: this checks core execution,
  // not pixels, GLES, or the production adaptor.
  Dali::TestApplication application;
  Dali::Actor actor = Dali::Actor::New();
  application.GetScene().Add(actor);
  actor.SetProperty(Dali::Actor::Property::POSITION, Dali::Vector3::ZERO);

  Dali::Animation animation = Dali::Animation::New(1.0f);
  animation.AnimateTo(Dali::Property(actor, Dali::Actor::Property::POSITION),
                      Dali::Vector3(100.0f, 0.0f, 0.0f),
                      Dali::AlphaFunction::LINEAR);
  animation.Play();
  application.SendNotification();
  application.Render(500);
  application.SendNotification();
  float midpoint = actor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::POSITION).x;
  application.Render(501);
  application.SendNotification();
  float endpoint = actor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::POSITION).x;

  if(std::fabs(midpoint - 50.0f) > 0.1f || std::fabs(endpoint - 100.0f) > 0.1f)
  {
    std::fprintf(stderr, "FAIL: animation midpoint=%f endpoint=%f\n", midpoint, endpoint);
    return 1;
  }
  std::printf("PASS: DALi Core WebAssembly actor animation midpoint=%.1f endpoint=%.1f (mock graphics)\n", midpoint, endpoint);
  return 0;
}
