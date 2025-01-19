#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#include <sstream>
#include <string>
#include <random>

using namespace std;
namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;

std::stringstream telemetry_stream;

namespace {
  void InitTracer() {
    auto exporter = std::make_unique<trace_exporter::OStreamSpanExporter>(telemetry_stream);
    auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
    std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processor));
    // Set the global trace provider
    trace_api::Provider::SetTracerProvider(provider);
  }
  void CleanupTracer() {
    std::shared_ptr<opentelemetry::trace::TracerProvider> none;
    trace_api::Provider::SetTracerProvider(none);
  }

  // Hypothetical processor function
  std::pair<int, std::string> processData() {
    // Simulate data processing
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);

    int processedValue = dist(gen);
    std::string processedStatus = (processedValue % 2 == 0) ? "even" : "odd";

    return {processedValue, processedStatus};
  }
}

class TelemetryHandler : public oatpp::web::server::HttpRequestHandler {
public:
  shared_ptr<OutgoingResponse> handle(const shared_ptr<IncomingRequest>& request) override {
    telemetry_stream.str(""); // Clear the previous telemetry data
    telemetry_stream.clear(); // Clear any error flags

    auto tracer = opentelemetry::trace::Provider::GetTracerProvider()->GetTracer("my-app-tracer");
    auto span = tracer->StartSpan("TelemetryRequest");

    // Process data and get results
    auto [processedValue, processedStatus] = processData();

    // Add custom attributes with processed data
    span->SetAttribute("custom.processed_value", processedValue);
    span->SetAttribute("custom.processed_status", processedStatus);

    // Add a custom log event with processed data
    span->AddEvent("ProcessedDataEvent", {{"event.processed_value", processedValue}, {"event.processed_status", processedStatus}});

    span->End();

    return ResponseFactory::createResponse(Status::CODE_200, telemetry_stream.str().c_str());
  }
};

void run() {
  auto router = oatpp::web::server::HttpRouter::createShared();
  router->route("GET", "/telemetry", std::make_shared<TelemetryHandler>());
  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
  auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", 8080, oatpp::network::Address::IP_4});
  oatpp::network::Server server(connectionProvider, connectionHandler);
  OATPP_LOGI("Telemetry Server", "Server running on port %s", static_cast<const char*>(connectionProvider->getProperty("port").getData()));
  server.run();
}

int main() {
  oatpp::base::Environment::init();
  InitTracer();
  run();
  oatpp::base::Environment::destroy();
  CleanupTracer();
  return 0;
}


