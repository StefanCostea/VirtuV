#pragma once

class PipelineStage {
public:
    virtual void process() = 0;
    virtual ~PipelineStage() = default;
};