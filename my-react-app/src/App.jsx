import React, { useState } from 'react';
import './App.css';

const colorPalette = [
  'linear-gradient(135deg, #4f46e5 0%, #6366f1 100%)',
  'linear-gradient(135deg, #0ea5e9 0%, #2563eb 100%)',
  'linear-gradient(135deg, #10b981 0%, #059669 100%)',
  'linear-gradient(135deg, #f59e0b 0%, #d97706 100%)',
  'linear-gradient(135deg, #ec4899 0%, #db2777 100%)',
  'linear-gradient(135deg, #8b5cf6 0%, #7c3aed 100%)',
  'linear-gradient(135deg, #14b8a6 0%, #0d9488 100%)',
  'linear-gradient(135deg, #f43f5e 0%, #e11d48 100%)'
];

export default function App() {
  const [theme, setTheme] = useState('dark');
  const [numProc, setNumProc] = useState(5);
  const [showForm, setShowForm] = useState(false);
  const [processes, setProcesses] = useState([]);
  const [algorithm, setAlgorithm] = useState('1');
  const [errorMsg, setErrorMsg] = useState('');
  const [results, setResults] = useState(null);

  const toggleTheme = () => {
    setTheme(prev => (prev === 'dark' ? 'light' : 'dark'));
  };

  const handleNextStep = () => {
    setErrorMsg('');
    const n = parseInt(numProc);
    if (isNaN(n) || n < 3 || n > 10) {
      setErrorMsg("Please enter a valid number of processes between 3 and 10.");
      return;
    }

    let initialProcs = [];
    for (let i = 0; i < n; i++) {
      initialProcs.push({
        pid: `P${i + 1}`,
        at: i === 0 ? 0 : i * 2,
        bt: Math.floor(Math.random() * 6) + 3
      });
    }
    setProcesses(initialProcs);
    setShowForm(true);
    setResults(null);
  };

  const updateProcessField = (index, field, value) => {
    const updated = [...processes];
    updated[index][field] = value;
    setProcesses(updated);
  };

  const runSimulation = () => {
    setErrorMsg('');
    let pids = new Set();
    let ats = new Set();
    let parsedProcs = [];

    for (let i = 0; i < processes.length; i++) {
      let pid = processes[i].pid.trim();
      let at = parseInt(processes[i].at);
      let bt = parseInt(processes[i].bt);

      if (!pid) {
        setErrorMsg(`Process ID at row ${i + 1} cannot be empty.`);
        return;
      }
      if (pids.has(pid)) {
        setErrorMsg(`Duplicate Process ID found: "${pid}". All IDs must be unique.`);
        return;
      }
      if (isNaN(at) || at < 0) {
        setErrorMsg(`Arrival time for ${pid} must be a non-negative number.`);
        return;
      }
      if (ats.has(at)) {
        setErrorMsg(`Duplicate Arrival time found (${at}). Arrival times must be unique.`);
        return;
      }
      if (isNaN(bt) || bt <= 0) {
        setErrorMsg(`Burst time for ${pid} must be greater than 0.`);
        return;
      }

      pids.add(pid);
      ats.add(at);
      parsedProcs.push({ pid, at, bt, originalIndex: i });
    }

    let simulated = [];
    if (algorithm === '1') {
      simulated = [...parsedProcs].sort((a, b) => a.at - b.at || a.originalIndex - b.originalIndex);
      let currentTime = 0;
      for (let p of simulated) {
        if (currentTime < p.at) currentTime = p.at;
        p.st = currentTime;
        p.ct = p.st + p.bt;
        p.tat = p.ct - p.at;
        p.wt = p.tat - p.bt;
        currentTime = p.ct;
      }
    } else {
      let currentTime = 0;
      let completedCount = 0;
      let uncompleted = parsedProcs.map(p => ({ ...p, completed: false }));
      let executed = [];

      while (completedCount < parsedProcs.length) {
        let available = uncompleted.filter(p => !p.completed && p.at <= currentTime);
        if (available.length === 0) {
          let nextAt = Math.min(...uncompleted.filter(p => !p.completed).map(p => p.at));
          currentTime = nextAt;
          continue;
        }
        available.sort((a, b) => a.bt - b.bt || a.at - b.at || a.originalIndex - b.originalIndex);
        let currentP = available[0];
        let targetIdx = uncompleted.findIndex(p => p.pid === currentP.pid);
        
        if (currentTime < uncompleted[targetIdx].at) {
          currentTime = uncompleted[targetIdx].at;
        }
        uncompleted[targetIdx].st = currentTime;
        uncompleted[targetIdx].ct = uncompleted[targetIdx].st + uncompleted[targetIdx].bt;
        uncompleted[targetIdx].tat = uncompleted[targetIdx].ct - uncompleted[targetIdx].at;
        uncompleted[targetIdx].wt = uncompleted[targetIdx].tat - uncompleted[targetIdx].bt;
        uncompleted[targetIdx].completed = true;

        currentTime = uncompleted[targetIdx].ct;
        executed.push(uncompleted[targetIdx]);
        completedCount++;
      }
      simulated = executed;
    }

    let totalWt = simulated.reduce((sum, p) => sum + p.wt, 0);
    let totalTat = simulated.reduce((sum, p) => sum + p.tat, 0);

    setResults({
      processes: simulated,
      avgWt: (totalWt / simulated.length).toFixed(2),
      avgTat: (totalTat / simulated.length).toFixed(2)
    });
  };

  return (
    <div className={`app-wrapper ${theme === 'light' ? 'light-theme' : 'dark-theme'}`}>
      <div className="app-container">
        <div className="app-header">
          <button className="theme-toggle-btn" onClick={toggleTheme}>
            <span>{theme === 'dark' ? '☀️' : '🌙'}</span> 
            <span>{theme === 'dark' ? 'Light Mode' : 'Dark Mode'}</span>
          </button>
          <h1>CPU CoreSim</h1>
          <p>Interactive Non-Preemptive FCFS & SJF Simulator</p>
        </div>

        <div className="card">
          <div className="card-title">Setup Configuration</div>
          <div className="form-grid">
            <div className="form-group">
              <label>Number of Processes (3 to 10)</label>
              <input 
                type="number" 
                min="3" 
                max="10" 
                value={numProc} 
                onChange={(e) => setNumProc(e.target.value)} 
              />
            </div>
            <button className="btn" onClick={handleNextStep}>Next Step</button>
          </div>
        </div>

        {showForm && (
          <div className="card">
            <div className="card-title">Process Parameters</div>
            {errorMsg && <div className="error-banner">{errorMsg}</div>}
            
            <div className="process-table-headers">
              <div>Process ID</div>
              <div>Arrival Time (AT)</div>
              <div>Burst Time (BT)</div>
            </div>

            {processes.map((p, idx) => (
              <div className="process-row" key={idx}>
                <input 
                  type="text" 
                  value={p.pid} 
                  onChange={(e) => updateProcessField(idx, 'pid', e.target.value)} 
                />
                <input 
                  type="number" 
                  min="0" 
                  value={p.at} 
                  onChange={(e) => updateProcessField(idx, 'at', parseInt(e.target.value) || 0)} 
                />
                <input 
                  type="number" 
                  min="1" 
                  value={p.bt} 
                  onChange={(e) => updateProcessField(idx, 'bt', parseInt(e.target.value) || 1)} 
                />
              </div>
            ))}

            <div style={{ marginTop: '24px' }} className="form-group">
              <label>Scheduling Algorithm</label>
              <select value={algorithm} onChange={(e) => setAlgorithm(e.target.value)}>
                <option value="1">First Come First Serve (FCFS)</option>
                <option value="2">Shortest Job First (SJF - Non-Preemptive)</option>
              </select>
            </div>

            <div style={{ marginTop: '24px' }}>
              <button className="btn" style={{ width: '100%' }} onClick={runSimulation}>
                Run Simulation & Generate Report
              </button>
            </div>
          </div>
        )}

        {results && (
          <div id="outputSection">
            <div className="metrics-grid">
              <div className="metric-card">
                <div className="label">Average Waiting Time</div>
                <div className="value">{results.avgWt} ms</div>
              </div>
              <div className="metric-card">
                <div className="label">Average Turnaround Time</div>
                <div className="value">{results.avgTat} ms</div>
              </div>
            </div>

            <div className="card">
              <div className="card-title">1st Step: Ready Queue Breakdown</div>
              <table>
                <thead>
                  <tr>
                    <th>Time (T)</th>
                    <th>Process ID</th>
                    <th>Burst Time (BT)</th>
                  </tr>
                </thead>
                <tbody>
                  {Array.from({ length: Math.max(...results.processes.map(p => p.at)) + 1 }, (_, t) => {
                    const match = results.processes.find(p => p.at === t);
                    return (
                      <tr key={t}>
                        <td><b>{t}</b></td>
                        <td>{match ? <span className="badge">{match.pid}</span> : '-'}</td>
                        <td>{match ? match.bt : '-'}</td>
                      </tr>
                    );
                  })}
                </tbody>
              </table>
            </div>

            <div className="card">
              <div className="card-title">2nd Step: Gantt Chart Visualization</div>
              <div className="gantt-container">
                <div className="gantt-timeline-track">
                  {results.processes.map((p, idx) => (
                    <div 
                      key={idx} 
                      className="gantt-block" 
                      style={{ background: colorPalette[idx % colorPalette.length] }}
                    >
                      <div className="pid">{p.pid}</div>
                      <div className="bt">BT: {p.bt}</div>
                    </div>
                  ))}
                </div>
                <div className="gantt-axis">
                  <span>{results.processes[0].st}</span>
                  {results.processes.map((p, idx) => (
                    <span key={idx}>{p.ct}</span>
                  ))}
                </div>
              </div>
            </div>

            <div className="card">
              <div className="card-title">3rd & 4th Step: Detailed Metrics Matrix</div>
              <table>
                <thead>
                  <tr>
                    <th>Process ID</th>
                    <th>Arrival Time</th>
                    <th>Burst Time</th>
                    <th>Waiting Time</th>
                    <th>Turnaround Time</th>
                  </tr>
                </thead>
                <tbody>
                  {results.processes.map((p, idx) => (
                    <tr key={idx}>
                      <td><span className="badge">{p.pid}</span></td>
                      <td>{p.at}</td>
                      <td>{p.bt}</td>
                      <td>{p.wt}</td>
                      <td>{p.tat}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}