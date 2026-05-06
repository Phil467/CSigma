#include "csGraphics.h"
#include "csScrollbar.h"
#include "csSection.h"
#include "csTypes.h"
#include "csUIObjects.h"
#include "csUniformListBox.h"
#include "csInput.h"
#include "csUtils.h"
#include "minwindef.h"
#include "windef.h"
#include "windows.h"
#include "csSubClasses.h"
#include "wingdi.h"
#include <algorithm>
#include <string>
#include <cwchar>
#include "csRuler.h"

using namespace CSSECMAN;

struct TaxBracketRow
{
    long double from;
    long double to;
    long double ratePercent;
};

/** Payroll deduction line for employee-side annual estimates (marginal bands on employment income). */
enum class SocialContributionProgram : int
{
    CPP = 0,
    CPP2 = 1,
    EI = 2,
    QPP = 3,
    QPP2 = 4,
    QPIP = 5,
};

/**
 * Same accumulation rule as income-tax brackets: for annual income I,
 * contribution += max(0, min(I, to) - from) * (ratePercent / 100).
 * Thresholds follow 2026 CRA / Revenu Québec public schedules (YMPE, MIE, etc.).
 */
struct SocialSecurityContributionRow
{
    SocialContributionProgram program;
    long double from;
    long double to;
    long double ratePercent;
};

/** Federal basic personal amount (2026 CRA indexation; max BPA for income at or below 29% bracket threshold). */
static constexpr long double kBasicPersonalAmount2026 = 16452.0L;
/** Sentinel upper bound for the top open-ended bracket. */
static constexpr long double kTaxBracketNoMax = 1e18L;

/* ---- 2026 employee payroll (CRA + Revenu Québec + RQAP; public schedules) ---- */
static constexpr long double kPayrollCppBasicExemption2026 = 3500.0L;
static constexpr long double kPayrollCppYmpe2026 = 74600.0L;
static constexpr long double kPayrollCppYampe2026 = 85000.0L;
static constexpr long double kPayrollCppEmployeeRate2026 = 5.95L;
static constexpr long double kPayrollCpp2EmployeeRate2026 = 4.00L;
static constexpr long double kPayrollEiMaxInsurable2026 = 68900.0L;
static constexpr long double kPayrollEiEmployeeRateRoc2026 = 1.63L;
static constexpr long double kPayrollEiEmployeeRateQuebec2026 = 1.30L;
/** QPP base + first additional, employee share (5.3% + 1.0% for 2026). */
static constexpr long double kPayrollQppEmployeeRate2026 = 6.30L;
static constexpr long double kPayrollQpp2EmployeeRate2026 = 4.00L;
static constexpr long double kPayrollQpipMaxInsurable2026 = 103000.0L;
static constexpr long double kPayrollQpipEmployeeRate2026 = 0.455L;

/* 2026 marginal brackets (federal/provincial; Canada.ca + Revenu Québec). */
vector<TaxBracketRow> g_federalTaxBrackets = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 58523.0L, 14.0L},
    {58524.0L, 117045.0L, 20.5L},
    {117046.0L, 181440.0L, 26.0L},
    {181441.0L, 258482.0L, 29.0L},
    {258483.0L, kTaxBracketNoMax, 33.0L},
};
vector<TaxBracketRow> g_provTaxBrackets_alberta = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 61200.0L, 8.0L},
    {61201.0L, 154259.0L, 10.0L},
    {154260.0L, 185111.0L, 12.0L},
    {185112.0L, 246813.0L, 13.0L},
    {246814.0L, 370220.0L, 14.0L},
    {370221.0L, kTaxBracketNoMax, 15.0L},
};
vector<TaxBracketRow> g_provTaxBrackets_british_columbia = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 50363.0L, 5.06L},
    {50364.0L, 100728.0L, 7.7L},
    {100729.0L, 115648.0L, 10.5L},
    {115649.0L, 140430.0L, 12.29L},
    {140431.0L, 190405.0L, 14.7L},
    {190406.0L, 265545.0L, 16.8L},
    {265546.0L, kTaxBracketNoMax, 20.5L},
};
vector<TaxBracketRow> g_provTaxBrackets_manitoba = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 47000.0L, 10.8L},
    {47001.0L, 100000.0L, 12.75L},
    {100001.0L, kTaxBracketNoMax, 17.4L},
};
vector<TaxBracketRow> g_provTaxBrackets_new_brunswick = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 52333.0L, 9.4L},
    {52334.0L, 104666.0L, 14.0L},
    {104667.0L, 193861.0L, 16.0L},
    {193862.0L, kTaxBracketNoMax, 19.5L},
};
vector<TaxBracketRow> g_provTaxBrackets_newfoundland_and_labrador = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 44678.0L, 8.7L},
    {44679.0L, 89354.0L, 14.5L},
    {89355.0L, 159528.0L, 15.8L},
    {159529.0L, 223340.0L, 17.8L},
    {223341.0L, 285319.0L, 19.8L},
    {285320.0L, 570638.0L, 20.8L},
    {570639.0L, 1141275.0L, 21.3L},
    {1141276.0L, kTaxBracketNoMax, 21.8L},
};
vector<TaxBracketRow> g_provTaxBrackets_northwest_territories = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 53003.0L, 5.9L},
    {53004.0L, 106009.0L, 8.6L},
    {106010.0L, 172346.0L, 12.2L},
    {172347.0L, kTaxBracketNoMax, 14.05L},
};
vector<TaxBracketRow> g_provTaxBrackets_nova_scotia = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 30995.0L, 8.79L},
    {30996.0L, 61991.0L, 14.95L},
    {61992.0L, 97417.0L, 16.67L},
    {97418.0L, 157124.0L, 17.5L},
    {157125.0L, kTaxBracketNoMax, 21.0L},
};
vector<TaxBracketRow> g_provTaxBrackets_nunavut = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 55801.0L, 4.0L},
    {55802.0L, 111602.0L, 7.0L},
    {111603.0L, 181439.0L, 9.0L},
    {181440.0L, kTaxBracketNoMax, 11.5L},
};
vector<TaxBracketRow> g_provTaxBrackets_ontario = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 53891.0L, 5.05L},
    {53892.0L, 107785.0L, 9.15L},
    {107786.0L, 150000.0L, 11.16L},
    {150001.0L, 220000.0L, 12.16L},
    {220001.0L, kTaxBracketNoMax, 13.16L},
};
vector<TaxBracketRow> g_provTaxBrackets_prince_edward_island = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 33928.0L, 9.5L},
    {33929.0L, 65820.0L, 13.47L},
    {65821.0L, 106890.0L, 16.6L},
    {106891.0L, 142250.0L, 17.62L},
    {142251.0L, kTaxBracketNoMax, 19.0L},
};
vector<TaxBracketRow> g_provTaxBrackets_quebec = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 54345.0L, 14.0L},
    {54346.0L, 108680.0L, 19.0L},
    {108681.0L, 132245.0L, 24.0L},
    {132246.0L, kTaxBracketNoMax, 25.75L},
};
vector<TaxBracketRow> g_provTaxBrackets_saskatchewan = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 54532.0L, 10.5L},
    {54533.0L, 155805.0L, 12.5L},
    {155806.0L, kTaxBracketNoMax, 14.5L},
};
vector<TaxBracketRow> g_provTaxBrackets_yukon = {
    {0.0L, kBasicPersonalAmount2026, 0.0L},
    {16453.0L, 58523.0L, 6.4L},
    {58524.0L, 117045.0L, 9.0L},
    {117046.0L, 181440.0L, 10.9L},
    {181441.0L, 500000.0L, 12.8L},
    {500001.0L, kTaxBracketNoMax, 15.0L},
};

/*********************** Social Security Contributions (2026 employee estimates) ***********************/

namespace
{
const SocialSecurityContributionRow kPayrollBandsRestOfCanada2026[] = {
    {SocialContributionProgram::CPP, kPayrollCppBasicExemption2026, kPayrollCppYmpe2026, kPayrollCppEmployeeRate2026},
    {SocialContributionProgram::CPP2, kPayrollCppYmpe2026, kPayrollCppYampe2026, kPayrollCpp2EmployeeRate2026},
    {SocialContributionProgram::EI, 0.0L, kPayrollEiMaxInsurable2026, kPayrollEiEmployeeRateRoc2026},
};

const SocialSecurityContributionRow kPayrollBandsQuebec2026[] = {
    {SocialContributionProgram::QPP, kPayrollCppBasicExemption2026, kPayrollCppYmpe2026, kPayrollQppEmployeeRate2026},
    {SocialContributionProgram::QPP2, kPayrollCppYmpe2026, kPayrollCppYampe2026, kPayrollQpp2EmployeeRate2026},
    {SocialContributionProgram::EI, 0.0L, kPayrollEiMaxInsurable2026, kPayrollEiEmployeeRateQuebec2026},
    {SocialContributionProgram::QPIP, 0.0L, kPayrollQpipMaxInsurable2026, kPayrollQpipEmployeeRate2026},
};
}


template<size_t N>
static vector<SocialSecurityContributionRow> vecFromPayrollRows(const SocialSecurityContributionRow (&rows)[N])
{
    return vector<SocialSecurityContributionRow>(rows, rows + N);
}

vector<SocialSecurityContributionRow> g_socialSecurityContributions_alberta = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_british_columbia = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_manitoba = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_new_brunswick = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_newfoundland_and_labrador = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_northwest_territories = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_nova_scotia = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_nunavut = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_ontario = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_prince_edward_island = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_quebec = vecFromPayrollRows(kPayrollBandsQuebec2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_saskatchewan = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);
vector<SocialSecurityContributionRow> g_socialSecurityContributions_yukon = vecFromPayrollRows(kPayrollBandsRestOfCanada2026);

int ROOT;
int secToolbarMenu, secMainWorkspace;
int secExpensesPanel, secUpperMiddlePanel, secChartCanvas, secTaxBracketsPanel, secTaskTimeline, monthPickerSection,
    secAddExpensestrip, secStatusBar, secCalculateButton;

int WEEKLY_WORKING_DAYS_ID, DAILY_SERVICES_ID, TOTAL_EXPENSES_ID;
int ANNUAL_EXPENSES_FIELD_ID;
int CP_ID, CAJ_ID, CAH_ID, CAM_ID, CAA_ID;
int CH_J_ID, CH_H_ID, CH_M_ID;
int MNJ_ID, MNH_ID, MNM_ID, MNA_ID;
int RRQ_RATE_FIELD_ID, RRQ2_RATE_FIELD_ID, RRQ_CEILING_FIELD_ID, RRQ2_CEILING_FIELD_ID, EI_RATE_FIELD_ID, EI_CEILING_FIELD_ID;
int ESTIMATED_FEDERAL_TAX_FIELD_ID, ESTIMATED_PROVINCIAL_TAX_FIELD_ID, ESTIMATED_RRQ_CONTRIB_FIELD_ID,
    ESTIMATED_EI_CONTRIB_FIELD_ID, ESTIMATED_QPIP_CONTRIB_FIELD_ID, ESTIMATED_TOTAL_TAX_FIELD_ID;

/** Doit rester égal à g_provTaxBrackets_newfoundland_and_labrador.size() (plus grande table provinciale). */
static constexpr int kMaxProvBracketUiRows = 9;
static constexpr int kMaxPayrollBandUiRows = 4;
static int g_provBracketFromIds[kMaxProvBracketUiRows];
static int g_provBracketToIds[kMaxProvBracketUiRows];
static int g_provBracketRateIds[kMaxProvBracketUiRows];
static int g_payrollBandFromIds[kMaxPayrollBandUiRows];
static int g_payrollBandToIds[kMaxPayrollBandUiRows];
static int g_payrollBandRateIds[kMaxPayrollBandUiRows];

void calculate(CSARGS Args);
void onRegionDependentFieldsRefresh(CSARGS Args);
void scrollingClientFunction(CSARGS Args);
void addExpensesFunction(CSARGS Args);
void goToDateFunction(CSARGS Args);
static long double parseNumericWide(const wstring& text);
long double getValue(CSINPUT* input, int id);
wchar_t* getStrNumericFormat(long double value, wchar_t* unit);

static long double marginalIncomeTaxFromBrackets(long double taxableIncome, const vector<TaxBracketRow>& brackets)
{
    if(taxableIncome <= 0.0L || brackets.empty())
        return 0.0L;
    long double tax = 0.0L;
    for(const TaxBracketRow& row : brackets)
    {
        long double hi = row.to;
        if(hi >= kTaxBracketNoMax - 1e15L)
            hi = taxableIncome + 1e15L;
        long double amt = std::min(taxableIncome, hi) - row.from;
        if(amt > 0.0L)
            tax += amt * (row.ratePercent / 100.0L);
    }
    return tax;
}

static const vector<TaxBracketRow>* provincialBracketsForRegion(long idx)
{
    static const vector<TaxBracketRow>* const tbl[] = {
        &g_provTaxBrackets_alberta,
        &g_provTaxBrackets_british_columbia,
        &g_provTaxBrackets_manitoba,
        &g_provTaxBrackets_new_brunswick,
        &g_provTaxBrackets_newfoundland_and_labrador,
        &g_provTaxBrackets_northwest_territories,
        &g_provTaxBrackets_nova_scotia,
        &g_provTaxBrackets_nunavut,
        &g_provTaxBrackets_ontario,
        &g_provTaxBrackets_prince_edward_island,
        &g_provTaxBrackets_quebec,
        &g_provTaxBrackets_saskatchewan,
        &g_provTaxBrackets_yukon,
    };
    constexpr long nProv = sizeof(tbl) / sizeof(tbl[0]);
    if(idx < 0 || idx >= nProv)
        idx = 8;
    return tbl[idx];
}

static const vector<SocialSecurityContributionRow>* socialContributionsForRegion(long idx)
{
    static const vector<SocialSecurityContributionRow>* const tbl[] = {
        &g_socialSecurityContributions_alberta,
        &g_socialSecurityContributions_british_columbia,
        &g_socialSecurityContributions_manitoba,
        &g_socialSecurityContributions_new_brunswick,
        &g_socialSecurityContributions_newfoundland_and_labrador,
        &g_socialSecurityContributions_northwest_territories,
        &g_socialSecurityContributions_nova_scotia,
        &g_socialSecurityContributions_nunavut,
        &g_socialSecurityContributions_ontario,
        &g_socialSecurityContributions_prince_edward_island,
        &g_socialSecurityContributions_quebec,
        &g_socialSecurityContributions_saskatchewan,
        &g_socialSecurityContributions_yukon,
    };
    constexpr long nProv = sizeof(tbl) / sizeof(tbl[0]);
    if(idx < 0 || idx >= nProv)
        idx = 8;
    return tbl[idx];
}

static long double marginalPayrollSlice(long double income, const SocialSecurityContributionRow& row)
{
    if(income <= 0.0L)
        return 0.0L;
    long double hi = row.to;
    if(hi >= kTaxBracketNoMax - 1e15L)
        hi = income + 1e15L;
    long double amt = std::min(income, hi) - row.from;
    if(amt > 0.0L)
        return amt * (row.ratePercent / 100.0L);
    return 0.0L;
}

static long double payrollMarginalPension(long double income, const vector<SocialSecurityContributionRow>& rows)
{
    long double sum = 0.0L;
    for(const SocialSecurityContributionRow& row : rows)
    {
        if(row.program == SocialContributionProgram::CPP || row.program == SocialContributionProgram::CPP2
            || row.program == SocialContributionProgram::QPP || row.program == SocialContributionProgram::QPP2)
            sum += marginalPayrollSlice(income, row);
    }
    return sum;
}

static long double payrollMarginalEi(long double income, const vector<SocialSecurityContributionRow>& rows)
{
    long double sum = 0.0L;
    for(const SocialSecurityContributionRow& row : rows)
    {
        if(row.program == SocialContributionProgram::EI)
            sum += marginalPayrollSlice(income, row);
    }
    return sum;
}

static long double payrollMarginalQpip(long double income, const vector<SocialSecurityContributionRow>& rows)
{
    long double sum = 0.0L;
    for(const SocialSecurityContributionRow& row : rows)
    {
        if(row.program == SocialContributionProgram::QPIP)
            sum += marginalPayrollSlice(income, row);
    }
    return sum;
}

void calculate(CSARGS Args)
{
    CSINPUT* inputExpenses = (CSINPUT*)Args[0];
    CSINPUT* accountingInput = (CSINPUT*)Args[1];
    CS_STRING_INCREMENTER_PARAMS* regionPickerParams = (CS_STRING_INCREMENTER_PARAMS*)Args[2];
    if(UINT(Args) == WM_LBUTTONDOWN)
    {
        long double totalExpenses = 0;
        for(int i=0; i<inputExpenses->getInputsNumber(); i++)
        {
            CSINPUT::CSINPUT_ENTITY_COLORS colors = inputExpenses->getTitleColors(i);
            if(colors.normal.r == 100 && colors.normal.g == 255 && colors.normal.b == 100)
            {
                wstring text = inputExpenses->getText(i);
                long double value = parseNumericWide(text);
                if(inputExpenses->getNoNameButtonState(i))
                {
                    long double njs = getValue(accountingInput, WEEKLY_WORKING_DAYS_ID);
                    long double npj = getValue(accountingInput, DAILY_SERVICES_ID);
                    if(njs < 1e-12L)
                        njs = 1e-12L;
                    value *= 52.0L * njs * npj;
                }
                totalExpenses += value;
            }
        }

        long double cp = getValue(accountingInput, CP_ID);
        long double npj = getValue(accountingInput, DAILY_SERVICES_ID);
        long double njs = getValue(accountingInput, WEEKLY_WORKING_DAYS_ID);
        if(njs < 1e-12L)
            njs = 1e-12L;
        if(npj < 0.0L)
            npj = 0.0L;

        const long double weeksPerYear = 52.0L;
        const long double daysPerYear = weeksPerYear * njs;
        const long double caa = cp * npj * njs * weeksPerYear;
        const long double caj = caa / daysPerYear;
        const long double cah = caa / weeksPerYear;
        const long double cam = caa / 12.0L;

        const long double chA = totalExpenses;
        const long double chJ = chA / daysPerYear;
        const long double chH = chA / weeksPerYear;
        const long double chM = chA / 12.0L;

        const long double taxableIncome = std::max(0.0L, caa - chA);

        long provinceIndex = 8;
        if(regionPickerParams)
            provinceIndex = regionPickerParams->currentItem;

        const long double federalTax = marginalIncomeTaxFromBrackets(taxableIncome, g_federalTaxBrackets);
        const vector<TaxBracketRow>* provBrackets = provincialBracketsForRegion(provinceIndex);
        const long double provincialTax = marginalIncomeTaxFromBrackets(taxableIncome, *provBrackets);

        const vector<SocialSecurityContributionRow>* payrollRows = socialContributionsForRegion(provinceIndex);
        const long double rrqContribution = payrollMarginalPension(taxableIncome, *payrollRows);
        const long double eiContribution = payrollMarginalEi(taxableIncome, *payrollRows);
        const long double qpipContribution = payrollMarginalQpip(taxableIncome, *payrollRows);

        const long double totalTaxEstimate = federalTax + provincialTax + rrqContribution + eiContribution + qpipContribution;
        const long double mna = caa - chA - totalTaxEstimate;
        const long double mnj = mna / daysPerYear;
        const long double mnh = mna / weeksPerYear;
        const long double mnm = mna / 12.0L;

        auto setMoney = [accountingInput](int fieldId, long double amount)
        {
            wchar_t* s = getStrNumericFormat(amount, L"$");
            accountingInput->setEditable(fieldId, true);
            accountingInput->setText(fieldId, s, true);
            accountingInput->setEditable(fieldId, false);
            free(s);
        };

        setMoney(ANNUAL_EXPENSES_FIELD_ID, chA);
        setMoney(CAA_ID, caa);
        setMoney(CAJ_ID, caj);
        setMoney(CAH_ID, cah);
        setMoney(CAM_ID, cam);
        setMoney(CH_J_ID, chJ);
        setMoney(CH_H_ID, chH);
        setMoney(CH_M_ID, chM);
        setMoney(ESTIMATED_FEDERAL_TAX_FIELD_ID, federalTax);
        setMoney(ESTIMATED_PROVINCIAL_TAX_FIELD_ID, provincialTax);
        setMoney(ESTIMATED_RRQ_CONTRIB_FIELD_ID, rrqContribution);
        setMoney(ESTIMATED_EI_CONTRIB_FIELD_ID, eiContribution);
        setMoney(ESTIMATED_QPIP_CONTRIB_FIELD_ID, qpipContribution);
        setMoney(ESTIMATED_TOTAL_TAX_FIELD_ID, totalTaxEstimate);
        setMoney(MNA_ID, mna);
        setMoney(MNJ_ID, mnj);
        setMoney(MNH_ID, mnh);
        setMoney(MNM_ID, mnm);
    }
}

static void refreshRegionTaxAndPayrollUi(CSINPUT* accountingInput, long provinceIndex)
{
    const vector<TaxBracketRow>* prov = provincialBracketsForRegion(provinceIndex);
    const size_t provN = prov->size();

    for(int i = 0; i < kMaxProvBracketUiRows; i++)
    {
        auto setProvField = [&](int fieldId, long double v, wchar_t* unit, bool isPercent)
        {
            accountingInput->setEditable(fieldId, true);
            if(i < int(provN))
            {
                wchar_t* s = getStrNumericFormat(v, unit);
                accountingInput->setText(fieldId, s, 0);
                free(s);
            }
            else if(isPercent)
                accountingInput->setText(fieldId, L"0 %", 0);
            else
                accountingInput->setText(fieldId, L"0 $", 0);
            accountingInput->setEditable(fieldId, false);
            accountingInput->update(fieldId);
        };
        if(i < int(provN))
        {
            setProvField(g_provBracketFromIds[i], (*prov)[size_t(i)].from, L"$", false);
            setProvField(g_provBracketToIds[i], (*prov)[size_t(i)].to, L"$", false);
            setProvField(g_provBracketRateIds[i], (*prov)[size_t(i)].ratePercent, L"%", true);
        }
        else
        {
            setProvField(g_provBracketFromIds[i], 0.0L, L"$", false);
            setProvField(g_provBracketToIds[i], 0.0L, L"$", false);
            setProvField(g_provBracketRateIds[i], 0.0L, L"%", true);
        }
    }

    const vector<SocialSecurityContributionRow>* pay = socialContributionsForRegion(provinceIndex);
    const size_t payN = pay->size();

    for(int r = 0; r < kMaxPayrollBandUiRows; r++)
    {
        auto setPayCell = [&](int fieldId, long double v, wchar_t* unit, bool active)
        {
            accountingInput->setEditable(fieldId, true);
            if(active)
            {
                wchar_t* s = getStrNumericFormat(v, unit);
                accountingInput->setText(fieldId, s, 0);
                free(s);
            }
            else
                accountingInput->setText(fieldId, L"-------", 0);
            accountingInput->setEditable(fieldId, false);
            accountingInput->update(fieldId);
        };
        const bool rowActive = r < int(payN);
        if(rowActive)
        {
            setPayCell(g_payrollBandFromIds[r], (*pay)[size_t(r)].from, L"$", true);
            setPayCell(g_payrollBandToIds[r], (*pay)[size_t(r)].to, L"$", true);
            setPayCell(g_payrollBandRateIds[r], (*pay)[size_t(r)].ratePercent, L"%", true);
        }
        else
        {
            setPayCell(g_payrollBandFromIds[r], 0.0L, L"$", false);
            setPayCell(g_payrollBandToIds[r], 0.0L, L"$", false);
            setPayCell(g_payrollBandRateIds[r], 0.0L, L"%", false);
        }
    }
}

void onRegionDependentFieldsRefresh(CSARGS Args)
{
    CSINPUT* accountingInput = (CSINPUT*)Args[0];
    CS_STRING_INCREMENTER_PARAMS* rp = (CS_STRING_INCREMENTER_PARAMS*)Args[1];
    int idCalculateButton = *(int*)Args[2];
    refreshRegionTaxAndPayrollUi(accountingInput, rp->currentItem);
    PostMessage(sHandle(idCalculateButton), WM_LBUTTONDOWN, 0, 0);
}




/**************************************************************** UI ****************************************************************/



CSIGMA_MAIN_BEGIN(L"fr-fr", L"fr-fr", 0, 0)

float geomCoef = 1.5;
setGeometryCoef(geomCoef);

ROOT = createSection(-1, {120, 40, 860, 485}, RGB(10, 10, 10), {1, 1, 1, 1, 1, 1, 1, 1});
enableDarkEdge(ROOT);
int rootWindowIconId = setIcon(ROOT, L"comptaResources/compta-logo.ico", L"comptaResources/compta-logo.ico", {2, 2, 21, 21});
setMinMaxInfo(ROOT, MINMAXINFO{.ptMinTrackSize = {720, 420}});
CSUIOBJECTS::addTitle(ROOT, L"COMPTA", {0}, 16, L"Arial Black", rootWindowIconId);
CSSYSCOMMAND_SECTION SYS_CMD = CSUIOBJECTS::addSysCommand(ROOT, {844}, "comptaResources/");

const COLORREF bg = RGB(10, 10, 10), bg2 = RGB(20, 20, 20), bg3 = RGB(30, 40, 30), bg4 = RGB(23, 23, 23), bg5 = RGB(15, 15, 15);
CSRGBA _bg3 = CSUTILS::toRGBA(bg3), _bg2 = CSUTILS::toRGBA(bg2), _bg = CSUTILS::toRGBA(bg), _bg4 = CSUTILS::toRGBA(bg4), _bg5 = CSUTILS::toRGBA(bg5);
const int topY = 25;

CSBIND_GEOM_PARAMS bgp;

int toolbarTotalWidth = 80 * 3 + 5;
secToolbarMenu = createSection(ROOT, {(800 - toolbarTotalWidth) / 2, 0, toolbarTotalWidth, 20}, RGB(20, 20, 50), {0});
bgp = {secToolbarMenu, {-0.5, 0, 0.5, 0}, {BIND_DEST_LEFT_EDGE, 0, BIND_DEST_LEFT_EDGE, 0}};
bindGeometry(ROOT, bgp);

csGraphics::setMouseWheelPreference(secToolbarMenu, CS_MOUSEWHEEL_NONE);
CSUNIFORMLISTBOX* mainMenuListBox = newUniformlListBoxPtr(&secToolbarMenu, 10, CS_ALIGN_HORIZONTAL);
mainMenuListBox->setDefaultFont(L"calibri",{12,0});
mainMenuListBox->setOffset({2,0});
mainMenuListBox->setMarging({2,0});
mainMenuListBox->setDefaultSize({80*geomCoef, 18*geomCoef});
mainMenuListBox->setDefaultTitleColors(RGB(220,220,220), RGB(100,220,100), RGB(100,250,100), RGB(0,0,0));
mainMenuListBox->setDefaultBackgroundColors(RGB(10, 10, 10), RGB(15,15,15), RGB(20,20,20), RGB(0,0,0));
mainMenuListBox->setIconSize(0,{18,18});

mainMenuListBox->setIcon(0, L"comptaResources/Settings01.bmp",L"comptaResources/Settings01.bmp", L"comptaResources/Settings01.bmp", L"comptaResources/Settings01.bmp");
mainMenuListBox->setMaxTextWidth(120*geomCoef);
mainMenuListBox->setDefaultTitle(L"Image");
mainMenuListBox->newItem(L"Previsionnel\0");
mainMenuListBox->newItem(L"Prestations\0");
mainMenuListBox->newItem(L"Factures\0");

mainMenuListBox->setActiveItem(0);
mainMenuListBox->create();

CS_STRING_INCREMENTER_PARAMS* regionPicker = CSUIOBJECTS::stringIncrementer(ROOT, {656, 0, 85+40, 21}, L"Alberta\0", 1, 1,
    {-1, 0, 1, 0}, {BIND_DEST_LEFT_EDGE, 0, BIND_DEST_LEFT_EDGE, 0}, "comptaResources/");
regionPicker->newItem(L"British Columbia\0", 1);
regionPicker->newItem(L"Manitoba\0", 1);
regionPicker->newItem(L"New Brunswick\0", 1);
regionPicker->newItem(L"Newfoundland and Labrador\0", 1);
regionPicker->newItem(L"Northwest Territories\0", 1);
regionPicker->newItem(L"Nova Scotia\0", 1);
regionPicker->newItem(L"Nunavut\0", 1);
regionPicker->newItem(L"Ontario\0", 1);
regionPicker->newItem(L"Prince Edward Island\0", 1);
regionPicker->newItem(L"Quebec\0", 1);
regionPicker->newItem(L"Saskatchewan\0", 1);
regionPicker->newItem(L"Yukon\0", 1);


secMainWorkspace = createSection(ROOT, {5, topY + 6, 840, 485-topY-20}, bg, {0});
bgp = {secMainWorkspace, {-1, -1, 1, 1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE, BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE}};
bindGeometry(ROOT, bgp);

secExpensesPanel = createSection(secMainWorkspace, {4, 6, 230-62, 369}, bg5, {0,0,1,0});
bgp = {secExpensesPanel, {0, -1, 0, 1}, {0, BIND_DEST_BOTTOM_EDGE, 0, BIND_DEST_BOTTOM_EDGE}};
bindGeometry(ROOT, bgp);

secUpperMiddlePanel = createSection(secMainWorkspace, {236-62, 6, 323+62, 178+110}, bg2, {0});
bgp = {secUpperMiddlePanel, {-1, -1, 1, 1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE, BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE}};
bindGeometry(ROOT, bgp);
bgp = {secUpperMiddlePanel, {0, 0, 1, 0}, {0, 0, BIND_DEST_RIGHT_EDGE|BIND_DEST_LEFT_EDGE, 0}};
bindGeometry(secExpensesPanel, bgp);

secChartCanvas = createSection(secUpperMiddlePanel, {2, 2, 323-4+62, 178-4+110}, bg4, {0});
bgp = {secChartCanvas, {-1, -1, 1, 1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE, BIND_DEST_RIGHT_EDGE, BIND_DEST_BOTTOM_EDGE}};
bindGeometry(ROOT, bgp);
bgp = {secChartCanvas, {0, 0, -1, 0}, {0, 0, BIND_DEST_RIGHT_EDGE, 0}};
bindGeometry(secExpensesPanel, bgp);

secTaxBracketsPanel = createSection(secMainWorkspace, {560, 6, 275, 178+110}, bg2, {1});
bgp = {secTaxBracketsPanel, {-1, -1, 1, 1}, {BIND_DEST_LEFT_EDGE, BIND_DEST_BOTTOM_EDGE, BIND_DEST_LEFT_EDGE, BIND_DEST_BOTTOM_EDGE}};
bindGeometry(ROOT, bgp);
bgp = {secUpperMiddlePanel, {1, 0, 0, 0}, BIND_DEST_RIGHT_EDGE,0,0,0};
bindGeometry(secTaxBracketsPanel, bgp);
bgp = {secChartCanvas, {1, 0, 0, 0}, BIND_DEST_RIGHT_EDGE,0,0,0};
bindGeometry(secTaxBracketsPanel, bgp);
setMinMaxInfo(secTaxBracketsPanel, MINMAXINFO{.ptMinTrackSize = {100, 0}, .ptMaxTrackSize = {500, 1000}});

secTaskTimeline = createSection(secMainWorkspace, {236-62, 185+110, 494+62, 235-2-110}, bg2, {0,1,0,0});
setMinMaxInfo(secTaskTimeline, MINMAXINFO{.ptMinTrackSize = {50, 50*geomCoef}, .ptMaxTrackSize = {10000, 200*geomCoef}});
bgp = {secTaskTimeline, {-1, -1, 1, 1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE, BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE}};
bindGeometry(ROOT, bgp);
bgp = {secTaskTimeline, {0, 0, 1, 0}, {0, 0, BIND_DEST_RIGHT_EDGE|BIND_DEST_LEFT_EDGE, 0}};
bindGeometry(secExpensesPanel, bgp);
bgp = {secUpperMiddlePanel, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE, 0, 0}};
bindGeometry(secTaskTimeline, bgp);
bgp = {secChartCanvas, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE, 0, 0}};
bindGeometry(secTaskTimeline, bgp);
bgp = {secTaxBracketsPanel, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE, 0, 0}};
bindGeometry(secTaskTimeline, bgp);

monthPickerSection = createSection(secMainWorkspace, {735, 185+110, 100, 244-10-110}, bg2, {1,1});
setMinMaxInfo(monthPickerSection, MINMAXINFO{.ptMinTrackSize = {50*geomCoef, 1}, .ptMaxTrackSize = {125*geomCoef, 235}});
bgp = {monthPickerSection, {-1, -1, 1, 1}, {BIND_DEST_LEFT_EDGE, BIND_DEST_TOP_EDGE, BIND_DEST_LEFT_EDGE, BIND_DEST_TOP_EDGE}};
bindGeometry(ROOT, bgp);
bgp = {secTaskTimeline, {1, 0, 0, 0}, BIND_DEST_RIGHT_EDGE,0,0,0};
bindGeometry(monthPickerSection, bgp);
bgp = {monthPickerSection, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE|BIND_DEST_TOP_EDGE, 0, 0}};
bindGeometry(secTaskTimeline, bgp);
bgp = {secTaskTimeline, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE|BIND_DEST_TOP_EDGE, 0, 0}};
bindGeometry(monthPickerSection, bgp);
bgp = {secUpperMiddlePanel, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE, 0, 0}};
bindGeometry(monthPickerSection, bgp);
bgp = {secChartCanvas, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE, 0, 0}};
bindGeometry(monthPickerSection, bgp);
bgp = {secTaxBracketsPanel, {0, 1, 0, 0}, {0, BIND_DEST_BOTTOM_EDGE, 0, 0}};
bindGeometry(monthPickerSection, bgp);


secAddExpensestrip = createSection(secMainWorkspace, {4, 384, 230-62, 32}, RGB(100,100,100), {0});
bgp = {secAddExpensestrip, {0, -1, 0, 1}, {0, BIND_DEST_TOP_EDGE, 0, BIND_DEST_TOP_EDGE}};
bindGeometry(ROOT, bgp);
bgp = {secAddExpensestrip, {0, 0, 1, 0}, {0, 0, BIND_DEST_RIGHT_EDGE, 0}};
bindGeometry(secExpensesPanel, bgp);

secStatusBar = createSection(secMainWorkspace, {2, 450-topY-6, 835, 25}, bg2, {0});
bgp = {secStatusBar, {-1, -1, 1, 1}, {BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE, BIND_DEST_RIGHT_EDGE, BIND_DEST_TOP_EDGE}};
bindGeometry(ROOT, bgp);


setTitle(secStatusBar, CSTEXT{.Text=L"...\0", .Font=L"Arial Black", .FontSize=14, .Italic=0, .Color={220,220,220},
    .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});

CSRGBA bgColor = {30,30,180}, highlightColor = {40,40,220}, clickColor = {35,35,205};
secCalculateButton = createSection(ROOT, {556, 0, 100, 20}, RGB(30,30,200), {0});
bgp = {secCalculateButton, {-1, 0, 1, 0}, {BIND_DEST_LEFT_EDGE, 0, BIND_DEST_LEFT_EDGE, 0}};
bindGeometry(ROOT, bgp);
setTitle(secCalculateButton, CSTEXT{.Text=L"Calculer\0", .Font=L"Arial Black", .FontSize=14, .Italic=0, .Color={220,220,220},
    .Marging={0,0}, .Align = CS_TA_CENTER, .Show=1});
CSUIFX::setBorderColorGradient(secCalculateButton, {20,20,20}, {20,20,20}, 2, 2);
setBorderThick(secCalculateButton, 1);
CSUIFX::setBackgroundColorGradient(secCalculateButton, highlightColor, bgColor, 2, 2);
CSUIFX::setBackgroundColorClickEffect(secCalculateButton, clickColor);


/************************************** Expenses **************************************/

auto templateInput = [](CSINPUT*& expensesInput, RECT geometry, wchar_t* title, int titleRectWidth=50)
{
    
    expensesInput->newInput(title,L"0 $",geometry, 0, titleRectWidth, 10);
    expensesInput->addNoNameButton(-1,L"comptaResources/greyPoint.bmp",L"comptaResources/greyPoint.bmp",L"comptaResources/greenPoint.bmp",L"comptaResources/greyPoint.bmp");
    expensesInput->addIncrementButtons(-1,L"comptaResources/ArrUp.bmp",L"comptaResources/ArrUp2.bmp",0,0, L"comptaResources/ArrDown.bmp",L"comptaResources/ArrDown2.bmp",0,0);
    expensesInput->setGBP(-1,0,0,2,0);
    expensesInput->setTextRectLeftMargin(-1, 10);
    expensesInput->setTextAlign(-1, CS_INPUT_TA_RIGHT);
    expensesInput->setAllowedChars(-1, L"-+0123456789.$ ");
};

RECT r = sRectClient(secExpensesPanel);

csGraphics::setGraphicAreaPosition(secExpensesPanel, {2,5});
csGraphics::setGraphicAreaColor(secExpensesPanel, {30,30,30}, {35,35,35});
csGraphics::setGraphicAreaSize(secExpensesPanel, {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)*2.5});
csGraphics::updateGraphicArea(secExpensesPanel, 1);
csGraphics::setMouseWheelPreference(secExpensesPanel, CS_MOUSEWHEEL_VSCROLL);

SetTimer(sHandle(secExpensesPanel), 0, 12,0);

CSINPUT* expensesInput = csNewInputContext(&secExpensesPanel);
expensesInput->setNoNameButtonVariableState(true);

int inputLeftX = 5, rowHeight = 25, stackTopY = 10;
for(int i=0; i<100; i++)
{
    templateInput(expensesInput, {inputLeftX, stackTopY + i * (rowHeight + 8), (r.right - 10), rowHeight + 4}, L"Nouvelle charge", 120);
    expensesInput->setSwitchable(-1, true);
    expensesInput->setTextColors(-1, {200,200,200}, {220,220,220}, {200,200,200}, {0,0,0});
    expensesInput->setFrameColors(-1, {10,10,10}, {20,20,20}, {30,30,30}, {0,0,0});
    expensesInput->setFrameBorderColors(-1, {10,10,10}, {100,100,100}, {100,100,30}, {0,0,0});
    expensesInput->setTitleColors(-1, {100,100,100}, {110,110,110}, {100,100,100}, {0,0,0});
    expensesInput->setTitleFrameBorderColors(-1, {50,50,100}, {10,10,10}, {10,10,10}, {0,0,0});
    expensesInput->setTitleFontSizeCoef(-1, 0.81);
    expensesInput->setTextFontSizeCoef(-1, 0.81);
    expensesInput->setEditable(-1, false);
    expensesInput->update(-1);
}

expensesInput->update();


/************************************** Tranches Provinciales **************************************/

auto templateAccountingInput = [](CSINPUT*& accountingInput, RECT geometry, wchar_t* title, int titleRectWidth=50)
{
    
    accountingInput->newInput(title,L"Nouvelle tranche...",geometry, 0, titleRectWidth, 10);
    accountingInput->setGBP(-1,1,0,1,0);
    accountingInput->setTextAlign(-1, CS_INPUT_TA_RIGHT);
    accountingInput->setAllowedChars(-1, L"-+0123456789.$ ");
    accountingInput->setTextFontSizeCoef(-1, 0.95f);
    accountingInput->setFrameColors(-1, {25,25,25}, {30,30,30}, {25,25,25}, {10,10,10});
    accountingInput->setFrameBorderColors(-1, {25,25,25}, {30,30,30}, {25,25,25}, {10,10,10});
    accountingInput->setTextColors(-1, {200,200,200}, {220,220,220}, {200,200,200}, {0,0,0});
    accountingInput->setTitleAlign(-1, CS_INPUT_TA_CENTER);
    accountingInput->update(-1);
};

r = sRectClient(secTaxBracketsPanel);

csGraphics::setGraphicAreaPosition(secTaxBracketsPanel, {2,5});
csGraphics::setGraphicAreaColor(secTaxBracketsPanel, {30,30,30}, {35,35,35});
csGraphics::setGraphicAreaSize(secTaxBracketsPanel, {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)*2.5});
csGraphics::updateGraphicArea(secTaxBracketsPanel, 1);
csGraphics::setMouseWheelPreference(secTaxBracketsPanel, CS_MOUSEWHEEL_VSCROLL);

SetTimer(sHandle(secTaxBracketsPanel), 0, 12,0);

CSINPUT* accountingInput = csNewInputContext(&secTaxBracketsPanel);
accountingInput->setLogSection(&secStatusBar);
inputLeftX += 5;

accountingInput->newInput(L" Tranches d'Imposition Fédérale",L" ",{inputLeftX,stackTopY+0*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->addUnrollButton(-1,L"comptaResources/hide.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp");
CSRGBA titleColor1 = {40,70,80}, titleColor2 = {40,70,90};
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {250,250,250}, {250,250,250}, {250,250,250}, {250,250,250});
accountingInput->setTitleFrameColors(-1, {50,50,200}, {50,50,220}, {50,50,200}, {50,50,200});
accountingInput->setFrameColors(-1, {50,50,220}, {50,50,240}, {50,50,200}, {50,50,50});
accountingInput->update(-1);

int nextFieldId = 1;

int maxFederalBracketsNum = 6;
int arrSize = g_federalTaxBrackets.size();

int fieldLength = (r.right-15)/3;

for(int i=0; i<maxFederalBracketsNum; i++)
{
    templateAccountingInput(accountingInput, {inputLeftX,stackTopY+(i+1)*(rowHeight+2), fieldLength+10,rowHeight}, L"De");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    if(i < arrSize)
        accountingInput->setText(nextFieldId, getStrNumericFormat(g_federalTaxBrackets[i].from, L"$"), 0);
    else
        accountingInput->setText(nextFieldId, L"-------");
    accountingInput->update(nextFieldId);
    nextFieldId++;
}

for(int i=0; i<maxFederalBracketsNum; i++)
{
    templateAccountingInput(accountingInput, {inputLeftX+fieldLength+10,stackTopY+(i+1)*(rowHeight+2), fieldLength+10,rowHeight}, L"A");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    if(i < arrSize)
        accountingInput->setText(nextFieldId, getStrNumericFormat(g_federalTaxBrackets[i].to, L"$"), 0);
    else
        accountingInput->setText(nextFieldId, L"-------");
    accountingInput->update(nextFieldId);
    nextFieldId++;
}

for(int i=0; i<maxFederalBracketsNum; i++)
{
    templateAccountingInput(accountingInput, {inputLeftX+(r.right-15)*2/3+20,stackTopY+(i+1)*(rowHeight+2), fieldLength-20,rowHeight}, L"=");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    accountingInput->replaceAllowedChar(nextFieldId, L'$', L'%');
    if(i < arrSize)
        accountingInput->setText(nextFieldId, getStrNumericFormat(g_federalTaxBrackets[i].ratePercent, L"%"), 0);
    else
        accountingInput->setText(nextFieldId, L"-------");
    accountingInput->update(nextFieldId);
    nextFieldId++;
}


/************************************************** Tranches Provinciales *******************************************************/

stackTopY += (maxFederalBracketsNum + 2)*(rowHeight+2);
accountingInput->newInput(L" Tranches d'Imposition Provinciales",L" ",{inputLeftX,stackTopY+0*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->addUnrollButton(-1,L"comptaResources/hide.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp");
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {250,250,250}, {250,250,250}, {250,250,250}, {250,250,250});
accountingInput->setTitleFrameColors(-1, {50,50,200}, {50,50,220}, {50,50,200}, {50,50,200});
accountingInput->setFrameColors(-1, {50,50,220}, {50,50,240}, {50,50,200}, {50,50,50});

nextFieldId++;

int maxProvBracketsNum = (int)g_provTaxBrackets_newfoundland_and_labrador.size();
arrSize = provincialBracketsForRegion(0)->size();
for(int i=0; i<maxProvBracketsNum; i++)
{
    g_provBracketFromIds[i] = nextFieldId;
    templateAccountingInput(accountingInput, {inputLeftX,stackTopY+(i+1)*(rowHeight+2), fieldLength+10,rowHeight}, L"De");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    if(i < arrSize)
        accountingInput->setText(nextFieldId, getStrNumericFormat((*provincialBracketsForRegion(0))[size_t(i)].from, L"$"), 0);
    else
        accountingInput->setText(nextFieldId, L"0 $");
    accountingInput->update(nextFieldId);
    nextFieldId++;
}

for(int i=0; i<maxProvBracketsNum; i++)
{
    g_provBracketToIds[i] = nextFieldId;
    templateAccountingInput(accountingInput, {inputLeftX+fieldLength+10,stackTopY+(i+1)*(rowHeight+2), fieldLength+10,rowHeight}, L"A");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    if(i < arrSize)
        accountingInput->setText(nextFieldId, getStrNumericFormat((*provincialBracketsForRegion(0))[size_t(i)].to, L"$"), 0);
    else
        accountingInput->setText(nextFieldId, L"0 $");
    accountingInput->update(nextFieldId);
    nextFieldId++;
}

for(int i=0; i<maxProvBracketsNum; i++)
{
    g_provBracketRateIds[i] = nextFieldId;
    templateAccountingInput(accountingInput, {inputLeftX+fieldLength*2+20,stackTopY+(i+1)*(rowHeight+2), fieldLength-20,rowHeight}, L"=");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    accountingInput->replaceAllowedChar(nextFieldId, L'$', L'%');
    if(i < arrSize)
        accountingInput->setText(nextFieldId, getStrNumericFormat((*provincialBracketsForRegion(0))[size_t(i)].ratePercent, L"%"), 0);
    else
        accountingInput->setText(nextFieldId, L"0 %");
    accountingInput->update(nextFieldId);
    nextFieldId++;
}


/********************************************************* Cotisations sociales ************************************************************ */

stackTopY += (maxProvBracketsNum+2)*(rowHeight+2);
accountingInput->newInput(L" Cotisations Sociales",L" ",{inputLeftX,stackTopY+0*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->addUnrollButton(-1,L"comptaResources/hide.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp");
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {250,250,250}, {250,250,250}, {250,250,250}, {250,250,250});
accountingInput->setTitleFrameColors(-1, {50,50,200}, {50,50,220}, {50,50,200}, {50,50,200});
accountingInput->setFrameColors(-1, {50,50,220}, {50,50,240}, {50,50,200}, {50,50,50});
accountingInput->update(-1);
nextFieldId++;
accountingInput->newInput(L" CPP / QPP ",L" ",{inputLeftX,stackTopY+1*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {100,100,100}, {100,100,100}, {100,100,100}, {100,100,100});
accountingInput->setTitleFrameColors(-1, _bg2, _bg2, _bg2, _bg2);
accountingInput->setFrameColors(-1, _bg2, _bg2, _bg2, _bg2);
accountingInput->update(-1);
nextFieldId++;

int len = (r.right-15)/2;

const vector<SocialSecurityContributionRow>* initPayrollBands = socialContributionsForRegion(0);
const size_t initPayN = initPayrollBands->size();

auto placePayrollBandRow = [&](int bandIndex, int yRow)
{
    g_payrollBandFromIds[bandIndex] = nextFieldId;
    templateAccountingInput(accountingInput, {inputLeftX, stackTopY + yRow * (rowHeight + 2), fieldLength + 10, rowHeight}, L"De");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    if(bandIndex < int(initPayN))
        accountingInput->setText(nextFieldId, getStrNumericFormat((*initPayrollBands)[size_t(bandIndex)].from, L"$"), 0);
    else
        accountingInput->setText(nextFieldId, L"-------", 0);
    accountingInput->update(nextFieldId);
    nextFieldId++;

    g_payrollBandToIds[bandIndex] = nextFieldId;
    templateAccountingInput(accountingInput, {inputLeftX + fieldLength + 10, stackTopY + yRow * (rowHeight + 2), fieldLength + 10, rowHeight}, L"A");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    if(bandIndex < int(initPayN))
        accountingInput->setText(nextFieldId, getStrNumericFormat((*initPayrollBands)[size_t(bandIndex)].to, L"$"), 0);
    else
        accountingInput->setText(nextFieldId, L"-------", 0);
    accountingInput->update(nextFieldId);
    nextFieldId++;

    g_payrollBandRateIds[bandIndex] = nextFieldId;
    templateAccountingInput(accountingInput, {inputLeftX + 2 * fieldLength + 20, stackTopY + yRow * (rowHeight + 2), fieldLength - 20, rowHeight}, L"=");
    accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
    accountingInput->replaceAllowedChar(nextFieldId, L'$', L'%');
    if(bandIndex < int(initPayN))
        accountingInput->setText(nextFieldId, getStrNumericFormat((*initPayrollBands)[size_t(bandIndex)].ratePercent, L"%"), 0);
    else
        accountingInput->setText(nextFieldId, L"-------", 0);
    accountingInput->update(nextFieldId);
    nextFieldId++;
};

placePayrollBandRow(0, 2);
placePayrollBandRow(1, 3);

accountingInput->newInput(L" Assurance emploi ",L" ",{inputLeftX,stackTopY+4*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {100,100,100}, {100,100,100}, {100,100,100}, {100,100,100});
accountingInput->setTitleFrameColors(-1, _bg2, _bg2, _bg2, _bg2);
accountingInput->setFrameColors(-1, _bg2, _bg2, _bg2, _bg2);
accountingInput->update(-1);
nextFieldId++;

placePayrollBandRow(2, 5);

accountingInput->newInput(L" RQAP / QPIP ",L" ",{inputLeftX,stackTopY+6*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {100,100,100}, {100,100,100}, {100,100,100}, {100,100,100});
accountingInput->setTitleFrameColors(-1, _bg2, _bg2, _bg2, _bg2);
accountingInput->setFrameColors(-1, _bg2, _bg2, _bg2, _bg2);
accountingInput->update(-1);
nextFieldId++;

placePayrollBandRow(3, 7);

RRQ_RATE_FIELD_ID = g_payrollBandRateIds[0];
RRQ2_RATE_FIELD_ID = g_payrollBandRateIds[1];
EI_RATE_FIELD_ID = g_payrollBandRateIds[2];

for(int i = 0; i < nextFieldId; i++)
    accountingInput->setEditable(i, false);

stackTopY += (9)*(rowHeight+2);

refreshRegionTaxAndPayrollUi(accountingInput, regionPicker->currentItem);
csSetUpdatingFunction(regionPicker->idText, onRegionDependentFieldsRefresh, 3, accountingInput, regionPicker, &secCalculateButton);

accountingInput->newInput(L" Données du Projet",L" ",{inputLeftX,stackTopY+0*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->addUnrollButton(-1,L"comptaResources/hide.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp");
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {250,250,250}, {250,250,250}, {250,250,250}, {250,250,250});
accountingInput->setTitleFrameColors(-1, {50,50,200}, {50,50,220}, {50,50,200}, {50,50,200});
accountingInput->setFrameColors(-1, {50,50,220}, {50,50,240}, {50,50,200}, {50,50,50});
accountingInput->update(-1);
nextFieldId++;

len = (r.right-15)/2;
int taxPanelRightColumnX = inputLeftX + len + 5;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+1*(rowHeight+2), len,rowHeight}, L"CP", 50);
accountingInput->addIncrementButtons(-1,L"comptaResources/ArrUp.bmp",L"comptaResources/ArrUp2.bmp",0,0, L"comptaResources/ArrDown.bmp",L"comptaResources/ArrDown2.bmp",0,0);
accountingInput->setGBP(nextFieldId, 1, 0, 1, 0);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"300 $");
accountingInput->update(nextFieldId);
CP_ID = nextFieldId++;
templateAccountingInput(accountingInput, {taxPanelRightColumnX,stackTopY+1*(rowHeight+2), len-5,rowHeight}, L"CAJ", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
CAJ_ID = nextFieldId++;
templateAccountingInput(accountingInput, {inputLeftX,stackTopY+2*(rowHeight+2), len,rowHeight}, L"NP / J", 50);
accountingInput->addIncrementButtons(-1,L"comptaResources/ArrUp.bmp",L"comptaResources/ArrUp2.bmp",0,0, L"comptaResources/ArrDown.bmp",L"comptaResources/ArrDown2.bmp",0,0);
accountingInput->setGBP(nextFieldId, 1, 0, 1, 0);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"1");
accountingInput->update(nextFieldId);
DAILY_SERVICES_ID = nextFieldId++;
templateAccountingInput(accountingInput, {taxPanelRightColumnX,stackTopY+2*(rowHeight+2), len-5,rowHeight}, L"CAH", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
CAH_ID = nextFieldId++;
templateAccountingInput(accountingInput, {inputLeftX,stackTopY+3*(rowHeight+2), len,rowHeight}, L"NJ / S", 50);
accountingInput->addIncrementButtons(-1,L"comptaResources/ArrUp.bmp",L"comptaResources/ArrUp2.bmp",0,0, L"comptaResources/ArrDown.bmp",L"comptaResources/ArrDown2.bmp",0,0);
accountingInput->setGBP(nextFieldId, 1, 0, 1, 0);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"5");
accountingInput->update(nextFieldId);
WEEKLY_WORKING_DAYS_ID = nextFieldId++;
templateAccountingInput(accountingInput, {taxPanelRightColumnX,stackTopY+3*(rowHeight+2), len-5,rowHeight}, L"CAM", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
CAM_ID = nextFieldId++;
templateAccountingInput(accountingInput, {inputLeftX,stackTopY+4*(rowHeight+2), len,rowHeight}, L"Ch. A", 50);
accountingInput->setTextColors(nextFieldId, {100,100,200}, {100,100,200}, {0});
accountingInput->setTitleColors(nextFieldId, {100,100,200}, {100,100,200});
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
ANNUAL_EXPENSES_FIELD_ID = nextFieldId++;
TOTAL_EXPENSES_ID = ANNUAL_EXPENSES_FIELD_ID;
templateAccountingInput(accountingInput, {taxPanelRightColumnX,stackTopY+4*(rowHeight+2), len-5,rowHeight}, L"CAA", 50);
accountingInput->setTextColors(nextFieldId, {100,100,200}, {100,100,200}, {0});
accountingInput->setTitleColors(nextFieldId, {100,100,200}, {100,100,200});
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
CAA_ID = nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+5*(rowHeight+2), len,rowHeight}, L"Ch. J", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
CH_J_ID = nextFieldId++;
templateAccountingInput(accountingInput, {inputLeftX,stackTopY+6*(rowHeight+2), len,rowHeight}, L"Ch. H", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
CH_H_ID = nextFieldId++;
templateAccountingInput(accountingInput, {inputLeftX,stackTopY+7*(rowHeight+2), len,rowHeight}, L"Ch. M", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
CH_M_ID = nextFieldId++;

templateAccountingInput(accountingInput, {taxPanelRightColumnX,stackTopY+5*(rowHeight+2), len-5,rowHeight}, L"MNJ", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
MNJ_ID = nextFieldId++;
templateAccountingInput(accountingInput, {taxPanelRightColumnX,stackTopY+6*(rowHeight+2), len-5,rowHeight}, L"MNH", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
MNH_ID = nextFieldId++;
templateAccountingInput(accountingInput, {taxPanelRightColumnX,stackTopY+7*(rowHeight+2), len-5,rowHeight}, L"MNM", 50);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
MNM_ID = nextFieldId++;

stackTopY += (9)*(rowHeight+2);
accountingInput->newInput(L" Impôts calculés",L" ",{inputLeftX,stackTopY+0*(rowHeight+2), (r.right-15),rowHeight}, 0, (r.right-15)-30, 10);
accountingInput->addUnrollButton(-1,L"comptaResources/hide.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp",L"comptaResources/hide1.bmp");
accountingInput->setGBP(-1,1,0,1,0);
accountingInput->setTitleColors(-1, {250,250,250}, {250,250,250}, {250,250,250}, {250,250,250});
accountingInput->setTitleFrameColors(-1, {50,50,200}, {50,50,220}, {50,50,200}, {50,50,200});
accountingInput->setFrameColors(-1, {50,50,220}, {50,50,240}, {50,50,200}, {50,50,50});
accountingInput->update(-1);
nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+1*(rowHeight+2), (r.right-15)-5,rowHeight}, L"Impôt fédéral", 130);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->setTitleAlign(nextFieldId, CS_INPUT_TA_LEFT);
accountingInput->update(nextFieldId);
ESTIMATED_FEDERAL_TAX_FIELD_ID = nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+2*(rowHeight+2), (r.right-15)-5,rowHeight}, L"Impôt provincial", 130);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->setTitleAlign(nextFieldId, CS_INPUT_TA_LEFT);
accountingInput->update(nextFieldId);
ESTIMATED_PROVINCIAL_TAX_FIELD_ID = nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+3*(rowHeight+2), (r.right-15)-5,rowHeight}, L"RRQ / CCP", 130);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->setTitleAlign(nextFieldId, CS_INPUT_TA_LEFT);
accountingInput->update(nextFieldId);
ESTIMATED_RRQ_CONTRIB_FIELD_ID = nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+4*(rowHeight+2), (r.right-15)-5,rowHeight}, L"Assurance emploi", 130);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->setTitleAlign(nextFieldId, CS_INPUT_TA_LEFT);
accountingInput->update(nextFieldId);
ESTIMATED_EI_CONTRIB_FIELD_ID = nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+5*(rowHeight+2), (r.right-15)-5,rowHeight}, L"RQAP / QPIP", 130);
accountingInput->setTitleColors(nextFieldId, titleColor1, titleColor2);
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->setTitleAlign(nextFieldId, CS_INPUT_TA_LEFT);
accountingInput->update(nextFieldId);
ESTIMATED_QPIP_CONTRIB_FIELD_ID = nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+6*(rowHeight+2), (r.right-15)-5,rowHeight}, L"Impôt total", 115);
accountingInput->setTitleColors(nextFieldId, {100,100,200}, {90,90,220});
accountingInput->setTextColors(nextFieldId, {100,100,200}, {100,100,200}, {0});
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->setTitleAlign(nextFieldId, CS_INPUT_TA_LEFT);
accountingInput->update(nextFieldId);
ESTIMATED_TOTAL_TAX_FIELD_ID = nextFieldId++;

templateAccountingInput(accountingInput, {inputLeftX,stackTopY+8*(rowHeight+2), 2*len,1.5*rowHeight}, L"MNA", 80);
accountingInput->setTitleColors(nextFieldId, {100,100,200}, {100,100,200});
accountingInput->setTextColors(nextFieldId, {100,100,200}, {100,100,200}, {0});
accountingInput->setText(nextFieldId, L"0 $");
accountingInput->update(nextFieldId);
MNA_ID = nextFieldId++;

const int kCalcFieldIds[] = {
    CAJ_ID, CAH_ID, CAM_ID, ANNUAL_EXPENSES_FIELD_ID, CAA_ID, CH_J_ID, CH_H_ID, CH_M_ID,
    MNJ_ID, MNH_ID, MNM_ID, MNA_ID,
    ESTIMATED_FEDERAL_TAX_FIELD_ID, ESTIMATED_PROVINCIAL_TAX_FIELD_ID, ESTIMATED_RRQ_CONTRIB_FIELD_ID,
    ESTIMATED_EI_CONTRIB_FIELD_ID, ESTIMATED_QPIP_CONTRIB_FIELD_ID, ESTIMATED_TOTAL_TAX_FIELD_ID,
};
for(int k = 0; k < int(sizeof(kCalcFieldIds) / sizeof(kCalcFieldIds[0])); k++)
    accountingInput->setEditable(kCalcFieldIds[k], false);

accountingInput->update();


/***************************************/ 
csGraphics::setGraphicAreaPosition(secAddExpensestrip, {2,5});
csGraphics::setGraphicAreaColor(secAddExpensestrip, {100,100,100}, {35,35,35});
csGraphics::setGraphicAreaSize(secAddExpensestrip, {GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)*2.5});
csGraphics::updateGraphicArea(secAddExpensestrip, 1);
csGraphics::setMouseWheelPreference(secAddExpensestrip, CS_MOUSEWHEEL_VSCROLL);

SetTimer(sHandle(secAddExpensestrip), 0, 12,0);

CSINPUT* addExpenses = csNewInputContext(&secAddExpensestrip);
addExpenses->newInput(L"Ajouter",L"Ajouter une charge...",{1,1,158*geomCoef,25*geomCoef}, 0, 2*geomCoef, 10);
addExpenses->addUnrollButton(-1,L"comptaResources/minus12.bmp",L"comptaResources/minus11.bmp",L"comptaResources/minus1.bmp",L"comptaResources/minus1.bmp");
addExpenses->addNoNameButton(-1,L"comptaResources/plus12.bmp",L"comptaResources/plus11.bmp",L"comptaResources/plus11.bmp",L"comptaResources/plus11.bmp");
addExpenses->setGBP(-1,0,0,2,0);
CSSECMAN::addAction(addExpenses->getId(), addExpensesFunction, 2, addExpenses, expensesInput);
addExpenses->update(-1);



/*********************************************** Mois **************************************/


csGraphics::setGraphicAreaPosition(monthPickerSection, {0,0});
csGraphics::setGraphicAreaColor(monthPickerSection, _bg2, {35,35,35});
csGraphics::setGraphicAreaSize(monthPickerSection, {125*geomCoef, 235*geomCoef});
csGraphics::updateGraphicArea(monthPickerSection, 1);
csGraphics::setMouseWheelPreference(monthPickerSection, CS_MOUSEWHEEL_VSCROLL);

CSUNIFORMLISTBOX* monthListBox = newUniformlListBoxPtr(&monthPickerSection, 1, CS_ALIGN_HORIZONTAL);
monthListBox->setDefaultFont(L"calibri",{12,0});
monthListBox->setOffset({2,0});
monthListBox->setMarging({5,5});
monthListBox->setDefaultSize({120*geomCoef, 18*geomCoef});
monthListBox->setDefaultTitleColors(RGB(220,220,220), RGB(220,220,220), RGB(80,80,250), RGB(0,0,0));
monthListBox->setDefaultBackgroundColors(bg2, RGB(_bg2.r+20,_bg2.g+20,_bg2.b+20), RGB(_bg2.r-10,_bg2.g-10,_bg2.b-10), RGB(0,0,0));
monthListBox->setIconSize(0,{18,18});

monthListBox->setIcon(0, L"comptaResources/Settings01.bmp",L"comptaResources/Settings01.bmp", L"comptaResources/Settings01.bmp", L"comptaResources/Settings01.bmp");
monthListBox->setMaxTextWidth(120*geomCoef);
monthListBox->setDefaultTitle(L"Image");
monthListBox->newItem(L"Janvier\0");
monthListBox->newItem(L"Février\0");
monthListBox->newItem(L"Mars\0");
monthListBox->newItem(L"Avril\0");
monthListBox->newItem(L"Mai\0");
monthListBox->newItem(L"Juin\0");
monthListBox->newItem(L"Juillet\0");
monthListBox->newItem(L"Août\0");
monthListBox->newItem(L"Septembre\0");
monthListBox->newItem(L"Octobre\0");
monthListBox->newItem(L"Novembre\0");
monthListBox->newItem(L"Décembre\0");

monthListBox->setActiveItem(0);
monthListBox->create();



/********************************** Taches **************************************/

csGraphics::setGraphicAreaPosition(secTaskTimeline, {2,5});
csGraphics::setGraphicAreaColor(secTaskTimeline, {30,30,30}, {35,35,35});
csGraphics::setGraphicAreaSize(secTaskTimeline, {4500*4, GetSystemMetrics(SM_CYSCREEN)/3});
csGraphics::updateGraphicArea(secTaskTimeline, 1);
csGraphics::setMouseWheelPreference(secTaskTimeline, CS_MOUSEWHEEL_HSCROLL);

CSSCROLLBAR hscrollTasks = CSUIOBJECTS::addHScrollBar(&secTaskTimeline, &secTaskTimeline, 0, 10, {0}, 5, (wchar_t*)(L"comptaResources/"));
CSSCROLLBAR vscrollTasks = CSUIOBJECTS::addVScrollBar(&secTaskTimeline, &secTaskTimeline, 0, 10);
hscrollTasks.setViewFrameBottomMarging(10);
vscrollTasks.setViewFrameRightMarging(10);
csGraphics::updateWhenGetResizing(secTaskTimeline);

const int timelineSlotCount = 7 - (365 % 7) + 365;

CSUNIFORMLISTBOX* taskGridListBox = newUniformlListBoxPtr(&secTaskTimeline, timelineSlotCount, CS_ALIGN_HORIZONTAL);
taskGridListBox->setDefaultFont(L"calibri",{12,0});
taskGridListBox->setOffset({2,5});
taskGridListBox->setMarging({1,5});
taskGridListBox->setDefaultSize({60, 30*geomCoef});

taskGridListBox->setDefaultTitleColors(RGB(220,50,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
taskGridListBox->setDefaultBackgroundColors(bg, RGB(_bg.r+20,_bg.g+20,_bg.b+20), RGB(_bg.r+30,_bg.g+30,_bg.b+30), RGB(0,0,0));

taskGridListBox->setIconSize(0,{18,18});
taskGridListBox->setIcon(0, L"comptaResources/Hide1.bmp",L"comptaResources/Hide2.bmp", L"comptaResources/Hide1.bmp", L"comptaResources/Hide1.bmp");

taskGridListBox->newIcon(L"comptaResources/Settings01.bmp", L"comptaResources/Settings02.bmp", L"comptaResources/Settings01.bmp", L"comptaResources/Settings01.bmp");
taskGridListBox->setIconSize(1,{8,8});

int monthBounds[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};


taskGridListBox->setDefaultTitleColors(RGB(220,50,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
taskGridListBox->setDefaultBackgroundColors(bg, RGB(_bg.r+20,_bg.g+20,_bg.b+20), RGB(_bg.r+30,_bg.g+30,_bg.b+30), RGB(0,0,0));


for(int i=0; i<timelineSlotCount; i++)
{
    if(i>=monthBounds[0] && i<monthBounds[1] || i >= monthBounds[12])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,30,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Jan\0", 1, 1);
    }

    if(i>=monthBounds[1] && i<monthBounds[2])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,30+200,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Fev\0", 1, 1);
    }

    if(i>=monthBounds[2] && i<monthBounds[3])
    {
        taskGridListBox->newItem(L"Mar\0", 1, 1);
        taskGridListBox->setDefaultTitleColors(RGB(220,30+40,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
    }

    if(i>=monthBounds[3] && i<monthBounds[4])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,20,220), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Avr\0", 1, 1);
    }

    if(i>=monthBounds[4] && i<monthBounds[5])
    {
        taskGridListBox->setDefaultTitleColors(RGB(20,50+60,100), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Mai\0", 1, 1);
    }
    
    if(i>=monthBounds[5] && i<monthBounds[6])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,30,100), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Juin\0", 1, 1);
    }

    if(i>=monthBounds[6] && i<monthBounds[7])
    {
        taskGridListBox->setDefaultTitleColors(RGB(0,30+120,200), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Juil\0", 1, 1);
    }

    if(i>=monthBounds[7] && i<monthBounds[8])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,30+100,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Aou\0", 1, 1);
    }
    
    if(i>=monthBounds[8] && i<monthBounds[9])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,30+180,100), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Sep\0", 1, 1);
    }

    if(i>=monthBounds[9] && i<monthBounds[10])
    {
        taskGridListBox->setDefaultTitleColors(RGB(0,30+120,200), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Oct\0", 1, 1);
    }

    if(i>=monthBounds[10] && i<monthBounds[11])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,30+40,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Nov\0", 1, 1);
    }

    if(i>=monthBounds[11] && i<monthBounds[12])
    {
        taskGridListBox->setDefaultTitleColors(RGB(220,30+220,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
        taskGridListBox->newItem(L"Dec\0", 1, 1);
    }


}

taskGridListBox->setMaxTextWidth(60);
taskGridListBox->setDefaultTitle(L"Image");
int days = 1;
vector<wchar_t*> daysNames = {L"Je ", L"Ve ", L"Sa ", L"Di ", L"Lu ", L"Ma ", L"Me "};

for(int i=0; i<timelineSlotCount; i++)
{
    if(i>=monthBounds[0] && i<monthBounds[1] || i >= monthBounds[12])
        taskGridListBox->setDefaultTitleColors(RGB(220,30,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[1] && i<monthBounds[2])
        taskGridListBox->setDefaultTitleColors(RGB(220,30+200,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[2] && i<monthBounds[3])
        taskGridListBox->setDefaultTitleColors(RGB(220,30+40,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[3] && i<monthBounds[4])
        taskGridListBox->setDefaultTitleColors(RGB(220,20,220), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[4] && i<monthBounds[5])
        taskGridListBox->setDefaultTitleColors(RGB(20,50+60,100), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
    
    if(i>=monthBounds[5] && i<monthBounds[6])
        taskGridListBox->setDefaultTitleColors(RGB(220,30,100), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[6] && i<monthBounds[7])
        taskGridListBox->setDefaultTitleColors(RGB(0,30+120,200), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[7] && i<monthBounds[8])
        taskGridListBox->setDefaultTitleColors(RGB(220,30+100,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));
    
    if(i>=monthBounds[8] && i<monthBounds[9])
        taskGridListBox->setDefaultTitleColors(RGB(220,30+180,100), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[9] && i<monthBounds[10])
        taskGridListBox->setDefaultTitleColors(RGB(0,30+120,200), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[10] && i<monthBounds[11])
        taskGridListBox->setDefaultTitleColors(RGB(220,30+40,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i>=monthBounds[11] && i<monthBounds[12])
        taskGridListBox->setDefaultTitleColors(RGB(220,30+220,50), RGB(220,50,50), RGB(250,100,100), RGB(0,0,0));

    if(i == monthBounds[0] || i == monthBounds[1] || i == monthBounds[2] || i == monthBounds[3] || i == monthBounds[4] || i == monthBounds[5] || i == monthBounds[6] || i == monthBounds[7] || i == monthBounds[8] || i == monthBounds[9] || i == monthBounds[10] || i == monthBounds[11] || i == monthBounds[12])
    {
        days = 1;
    }
    int index = i%7;
    taskGridListBox->newItem((wchar_t*)(wstring(daysNames[index]) + to_wstring(days++)).c_str(), 1, 1);
}

taskGridListBox->setDefaultTitleColors(RGB(220,220,220), RGB(220,220,220), RGB(250,100,100), RGB(0,0,0));
taskGridListBox->setDefaultBackgroundColors(bg2, RGB(_bg2.r+20,_bg2.g+20,_bg2.b+20), RGB(_bg2.r+30,_bg2.g+30,_bg2.b+30), RGB(0,0,0));

for(int j=0; j<8; j++)
{
    for(int i=0; i<timelineSlotCount; i++)
    {
        if(i%2==0)
        {
            taskGridListBox->setDefaultBackgroundColors(RGB(25,25,25), RGB(30,30,30), RGB(40,40,40), RGB(0,0,0));
        }
        else
        {
            taskGridListBox->setDefaultBackgroundColors(RGB(30,30,30), RGB(40,40,40), RGB(34,34,34), RGB(0,0,0));

        }
        taskGridListBox->newItem((wchar_t*)(to_wstring(j+8) + L"H\0").c_str());
    }
}


taskGridListBox->setActiveItem(0);
taskGridListBox->create();

hscrollTasks.setScrollingClientFunction(scrollingClientFunction, 2, monthListBox, taskGridListBox);

addAction(monthPickerSection, goToDateFunction, 3, monthListBox, taskGridListBox, &hscrollTasks);
addAction(secCalculateButton, calculate, 3, expensesInput, accountingInput, regionPicker);

csGraphics::setGraphicAreaPosition(secChartCanvas, {30, 30});
csGraphics::setGraphicAreaColor(secChartCanvas, _bg4, {35,35,35});
csGraphics::setGraphicAreaSize(secChartCanvas, {2000, 1200});
csGraphics::updateGraphicArea(secChartCanvas, 1);
csGraphics::setMouseWheelPreference(secChartCanvas, CS_MOUSEWHEEL_VSCROLL);

CSRULER* rulerX = csNewRuler(secChartCanvas, {30, 0}, {2000, 30}, CS_RULER_TEXT_UNDER, -100, 5, METER, 5, 5);
rulerX->setColors({25,25,25}, {30,30,30}, {100,100,100}, {150,150,150}, {100,100,100});
rulerX->update();
 
CSRULER* rulerY = csNewRuler(secChartCanvas, {0, 30}, {30, 1200}, CS_RULER_TEXT_UNDER, -100, 5, METER, 5, 5);
rulerY->setColors({25,25,25}, {30,30,30}, {100,100,100}, {150,150,150}, {100,100,100});
rulerY->update();


CSIGMA_MAIN_END()



/****************************************************************** Utilities ************************************************************************* */



void addExpensesFunction(CSARGS Args)
{
    CSINPUT* sourceInput = (CSINPUT*)Args[0];
    CSINPUT* targetInput = (CSINPUT*)Args[1];

    if((UINT)Args == WM_LBUTTONDOWN)
    {
        if(sourceInput->isMouseHoveringNoNameButton(0) && sourceInput->getActiveInputCharNumber() > 0)
        {
            wstring value = sourceInput->getText(0);
            int targetFieldId = targetInput->getActiveInputId();
            int pxLength = CSUTILS::textExtentW(targetInput->getId(), targetInput->getFont(targetFieldId), (wchar_t*)(value.c_str()))->cx;
            targetInput->setTitleRectWidth(targetFieldId, pxLength + 15);
            targetInput->setGBP(targetFieldId, 0, 0, 2, 0);
            targetInput->setTitle(targetFieldId, (wchar_t*)(value.c_str()));
            targetInput->setTitleColors(targetFieldId, {100,255,100}, {100,255,100}, {100,255,100}, {0,0,0});
            targetInput->setEditable(targetFieldId, true);
            targetInput->update(targetFieldId);
        }
    }
}

void goToDateFunction(CSARGS Args)
{
    CSUNIFORMLISTBOX* monthPickerList = (CSUNIFORMLISTBOX*)Args[0];
    CSUNIFORMLISTBOX* taskGridListBox = (CSUNIFORMLISTBOX*)Args[1];
    CSSCROLLBAR* hScroll = (CSSCROLLBAR*)Args[2];

    if(UINT(Args) != WM_LBUTTONDOWN)
        return;

    static const int monthStartDay[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    const int taskSectionId = taskGridListBox->getId();
    const int activeMonth = *monthPickerList->getActiveItem();
    const RECT itemRect = taskGridListBox->getItemRect(0);

    if(activeMonth < 0 || activeMonth >= 12)
        return;

    const int timelineSlotCount = 7 - (365 % 7) + 365;
    const int vScrollWidth = 10;
    const int cellGap = 1;
    const RECT clientRect = sRectClient(taskSectionId);
    const int visibleSlots = int(1.0 * (clientRect.right - clientRect.left - vScrollWidth)
        / (itemRect.right - itemRect.left + cellGap));
    const float scrollRatio = float(monthStartDay[activeMonth]) / float(timelineSlotCount - visibleSlots);
    hScroll->setPositionRatio(scrollRatio);
}

void scrollingClientFunction(CSARGS Args)
{
    CSUNIFORMLISTBOX* monthPickerList = (CSUNIFORMLISTBOX*)Args[0];
    CSUNIFORMLISTBOX* taskGridListBox = (CSUNIFORMLISTBOX*)Args[1];

    const RECT itemRect = taskGridListBox->getItemRect(0);
    const int cellGap = 1;
    const int cellPitch = itemRect.right - itemRect.left + cellGap;

    static const int monthStartDay[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    const int taskSectionId = taskGridListBox->getId();
    const int graphicAreaX = csGraphics::getGraphicAreaInXPos(taskSectionId);

    static int lastSyncedMonth = 0;

    for(int m = 0; m < 12; m++)
    {
        if(graphicAreaX >= monthStartDay[m] * cellPitch && graphicAreaX < monthStartDay[m + 1] * cellPitch && lastSyncedMonth != m)
        {
            lastSyncedMonth = m;
            monthPickerList->setActiveItem(m);
            monthPickerList->update();
            break;
        }
    }
}

wchar_t* getStrNumericFormat(long double value, wchar_t* unit)
{
    wchar_t raw[100] = {0};
    constexpr size_t kMoneyStrChars = 100;
    if((long long)(value * 100) % 100 == 0)
        swprintf(raw, kMoneyStrChars, L"%lld", static_cast<long long>(value));
    else
        swprintf(raw, kMoneyStrChars, L"%.2Lf", value);

    wstring number(raw);
    bool isNegative = false;
    if(!number.empty() && number[0] == L'-')
    {
        isNegative = true;
        number.erase(0, 1);
    }

    size_t dotPos = number.find(L'.');
    wstring integerPart = (dotPos == wstring::npos) ? number : number.substr(0, dotPos);
    wstring fractionalPart = (dotPos == wstring::npos) ? L"" : number.substr(dotPos);

    wstring grouped;
    grouped.reserve(integerPart.size() + integerPart.size() / 3 + 4);
    if(isNegative)
        grouped.push_back(L'-');

    for(size_t i = 0; i < integerPart.size(); ++i)
    {
        grouped.push_back(integerPart[i]);
        size_t remaining = integerPart.size() - i - 1;
        if(remaining > 0 && remaining % 3 == 0)
            grouped.push_back(L' ');
    }

    wstring outputStr = grouped + fractionalPart + L" " + wstring(unit);
    wchar_t* output = csAlloc<wchar_t>(outputStr.size() + 1);
    wcscpy(output, outputStr.c_str());
    return output;
}

/** Parses a number from UI text (spaces, currency, grouping, FR/US separators). */
static long double parseNumericWide(const wstring& text)
{
    wstring raw;
    raw.reserve(text.size());
    for(wchar_t c : text)
    {
        if(c == L' ' || c == L'\t' || c == L'\u00A0' || c == L'\u202F')
            continue;
        if(c >= L'0' && c <= L'9')
            raw.push_back(c);
        else if(c == L'-' && raw.empty())
            raw.push_back(c);
        else if(c == L'.' || c == L',')
            raw.push_back(c);
    }
    if(raw.empty() || raw == L"-")
        return 0.0L;

    const bool hasDot = raw.find(L'.') != wstring::npos;
    size_t commaCount = 0;
    for(wchar_t c : raw)
    {
        if(c == L',')
            commaCount++;
    }
    if(hasDot)
    {
        wstring t;
        for(wchar_t c : raw)
        {
            if(c != L',')
                t.push_back(c);
        }
        raw.swap(t);
    }
    else if(commaCount == 1)
    {
        for(size_t i = 0; i < raw.size(); ++i)
        {
            if(raw[i] == L',')
                raw[i] = L'.';
        }
    }
    else if(commaCount > 1)
    {
        wstring t;
        for(wchar_t c : raw)
        {
            if(c != L',')
                t.push_back(c);
        }
        raw.swap(t);
    }

    wchar_t* endPtr = nullptr;
    const long double v = std::wcstold(raw.c_str(), &endPtr);
    if(endPtr == raw.c_str())
        return 0.0L;
    return v;
}

long double getValue(CSINPUT* input, int id)
{
    return parseNumericWide(input->getText(id));
}

