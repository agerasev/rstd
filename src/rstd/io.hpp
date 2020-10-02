#pragma once

#include <iostream>


std::ostream &stdout_();
std::ostream &stderr_();

void set_stdout(std::ostream &ostream);
void set_stderr(std::ostream &ostream);
